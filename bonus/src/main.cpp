/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** Professional GUI Client with Robust Context and Advanced Parsing
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <algorithm>
#include <sstream>
#include "../include/socket.hpp"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

struct Color { Uint8 r, g, b, a; };
const Color BG_COLOR = {15, 23, 42, 255};
const Color SIDEBAR_COLOR = {10, 15, 30, 255};
const Color EXPLORER_BG = {15, 20, 35, 255};
const Color INPUT_BG = {30, 41, 59, 255};
const Color ACCENT = {99, 102, 241, 255};
const Color TEXT_WHITE = {248, 250, 252, 255};
const Color TEXT_DIM = {148, 163, 184, 255};
const Color MODAL_BG = {10, 15, 30, 240};

struct Entity {
    std::string type, uuid, name, desc;
};

struct Button {
    SDL_Rect rect;
    std::string text, command;
};

void draw_rect(SDL_Renderer* ren, int x, int y, int w, int h, Color c, bool fill = true) {
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
    SDL_Rect r = {x, y, w, h};
    if (fill) SDL_RenderFillRect(ren, &r);
    else SDL_RenderDrawRect(ren, &r);
}

void draw_text(SDL_Renderer* ren, TTF_Font* font, const std::string& text, int x, int y, Color c) {
    if (text.empty()) return;
    SDL_Color sc = {c.r, c.g, c.b, c.a};
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), sc);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

bool is_in_rect(int x, int y, const SDL_Rect& r) {
    return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> tokens; std::string token; std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delim)) tokens.push_back(token);
    return tokens;
}

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) return 1;

    SDL_Window* win = SDL_CreateWindow("MyTeams - Explorer Professional", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/Adwaita/AdwaitaMono-Bold.ttf", 15);
    TTF_Font *font_bold = TTF_OpenFont("/usr/share/fonts/Adwaita/AdwaitaMono-Bold.ttf", 18);

    std::string ip = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? std::stoi(argv[2]) : 4242;

    BonusSocket socket;
    enum State { DISCONNECTED, CONNECTED, ERROR };
    State current_state = DISCONNECTED;
    if (socket.connect_to(ip, port)) current_state = CONNECTED;
    else current_state = ERROR;
    
    std::vector<std::string> nav_stack; // Uuids context
    std::vector<Entity> explorer_items;
    std::deque<std::string> logs;
    logs.push_back("Connected to " + ip + ":" + std::to_string(port));

    bool show_create_modal = false;
    bool show_item_modal = false;
    Entity selected_entity;
    std::string modal_title = "", field1_name = "", field2_name = "";
    std::string field1_buf = "", field2_buf = "";
    int active_field = 0;
    std::string input_text = "";

    std::vector<Button> nav_buttons = {
        {{20, 60, 260, 35}, "USERS LIST", "/users"},
        {{20, 105, 260, 35}, "MY INFO", "/info"},
        {{20, 150, 260, 35}, "SUBSCRIBED", "/subscribed"},
        {{20, 195, 260, 35}, "LOGOUT", "/logout"}
    };

    bool running = true;
    SDL_Event ev;
    SDL_StartTextInput();

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = false;
            if (ev.type == SDL_MOUSEBUTTONDOWN) {
                int mx, my; SDL_GetMouseState(&mx, &my);
                if (show_create_modal) {
                    if (is_in_rect(mx, my, {420, 520, 160, 50})) { // CONFIRM
                        std::string cmd = (nav_stack.size() == 3) ? ("/create \"" + field1_buf + "\"") : ("/create \"" + field1_buf + "\" \"" + field2_buf + "\"");
                        socket.send_data(cmd + "\n");
                        explorer_items.clear(); socket.send_data("/list\n");
                        show_create_modal = false; field1_buf = ""; field2_buf = "";
                    } else if (is_in_rect(mx, my, {620, 520, 160, 50})) show_create_modal = false;
                    else if (is_in_rect(mx, my, {420, 290, 360, 40})) active_field = 0;
                    else if (is_in_rect(mx, my, {420, 390, 360, 40})) active_field = 1;
                    } else if (show_item_modal) {
                        if (is_in_rect(mx, my, {420, 300, 360, 45})) { // ENTER
                            if (selected_entity.type != "REPLY") {
                                if (nav_stack.size() < 3) nav_stack.push_back(selected_entity.uuid);
                                std::string cmd = "/use";
                                for (const auto& u : nav_stack) cmd += " \"" + u + "\"";
                                socket.send_data(cmd + "\n");
                                explorer_items.clear(); socket.send_data("/list\n");
                                show_item_modal = false;
                            }
                        } else if (is_in_rect(mx, my, {420, 355, 360, 45})) { // SUBSCRIBE
                            if (selected_entity.type == "TEAM") {
                                socket.send_data("/subscribe \"" + selected_entity.uuid + "\"\n");
                                show_item_modal = false;
                            }
                        } else if (is_in_rect(mx, my, {420, 410, 360, 45})) { // UNSUBSCRIBE
                            if (selected_entity.type == "TEAM") {
                                socket.send_data("/unsubscribe \"" + selected_entity.uuid + "\"\n");
                                show_item_modal = false;
                            }
                        } else if (is_in_rect(mx, my, {420, 470, 360, 45})) show_item_modal = false;
                    }
 else {
                    if (mx > 15 && mx < 285 && my > 330 && my < 710) {
                        int idx = (my - 340) / 42;
                        if (idx >= 0 && idx < (int)explorer_items.size()) {
                            selected_entity = explorer_items[idx];
                            show_item_modal = true;
                        }
                    }
                    for (auto& b : nav_buttons) if (is_in_rect(mx, my, b.rect)) {
                        if (current_state != CONNECTED) { if (socket.connect_to(ip, port)) current_state = CONNECTED; else current_state = ERROR; }
                        if (current_state == CONNECTED) socket.send_data(b.command + "\n");
                    }
                    if (my < 50 && mx > 300 && mx < 450) { // Click GLOBAL breadcrumb
                        nav_stack.clear(); socket.send_data("/use\n"); explorer_items.clear(); socket.send_data("/list\n");
                    }
                    if (is_in_rect(mx, my, {20, 250, 260, 40})) {
                        show_create_modal = true; active_field = 0;
                        if (nav_stack.size() == 0) { modal_title = "NEW TEAM"; field1_name = "Team Name"; field2_name = "Description"; }
                        else if (nav_stack.size() == 1) { modal_title = "NEW CHANNEL"; field1_name = "Channel Name"; field2_name = "Description"; }
                        else if (nav_stack.size() == 2) { modal_title = "NEW THREAD"; field1_name = "Title"; field2_name = "Initial Message"; }
                        else { modal_title = "NEW REPLY"; field1_name = "Message Body"; field2_name = "(N/A)"; }
                    }
                }
            }
            if (ev.type == SDL_TEXTINPUT) {
                if (show_create_modal) { if (active_field == 0) field1_buf += ev.text.text; else field2_buf += ev.text.text; }
                else input_text += ev.text.text;
            } else if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_BACKSPACE) {
                    if (show_create_modal) { if (active_field == 0 && !field1_buf.empty()) field1_buf.pop_back(); else if (active_field == 1 && !field2_buf.empty()) field2_buf.pop_back(); }
                    else if (!input_text.empty()) input_text.pop_back();
                } else if (ev.key.keysym.sym == SDLK_TAB && show_create_modal) active_field = 1 - active_field;
                else if (ev.key.keysym.sym == SDLK_RETURN && !show_create_modal) {
                    if (current_state != CONNECTED) { if (socket.connect_to(ip, port)) current_state = CONNECTED; else current_state = ERROR; }
                    if (current_state == CONNECTED) {
                        logs.push_back("> " + input_text);
                        if (input_text.find("/use") == 0) {
                            nav_stack.clear();
                            std::vector<std::string> parts = split(input_text, ' ');
                            for (size_t i = 1; i < parts.size(); ++i) {
                                std::string u = parts[i];
                                if (u.front() == '"') u = u.substr(1, u.length()-2);
                                nav_stack.push_back(u);
                            }
                        }
                        socket.send_data(input_text + "\n");
                        if (input_text.find("/use") == 0 || input_text.find("/login") == 0) { explorer_items.clear(); socket.send_data("/list\n"); }
                        input_text = "";
                    }
                }
            }
        }

        if (current_state == CONNECTED) {
            socket.has_data(); std::string line;
            while (!(line = socket.receive_line()).empty()) {
                std::vector<std::string> tabs = split(line, '\t');
                if (tabs.size() >= 3 && (tabs[0] == "TEAM" || tabs[0] == "CHANNEL" || tabs[0] == "THREAD" || tabs[0] == "REPLY")) {
                    Entity e; e.type = tabs[0]; e.uuid = tabs[1];
                    if (e.type == "THREAD") e.name = (tabs.size() >= 5) ? tabs[4] : tabs[2];
                    else if (e.type == "REPLY") e.name = (tabs.size() >= 5) ? tabs[4] : tabs[2];
                    else e.name = tabs[2];
                    explorer_items.push_back(e);
                } else { logs.push_back(line); if (logs.size() > 28) logs.pop_front(); }
            }
            if (!socket.is_connected()) current_state = DISCONNECTED;
        }

        SDL_SetRenderDrawColor(ren, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a); SDL_RenderClear(ren);
        draw_rect(ren, 0, 0, 300, WINDOW_HEIGHT, SIDEBAR_COLOR);
        draw_text(ren, font_bold, "CONTROLS", 30, 25, ACCENT);
        for (auto& b : nav_buttons) { draw_rect(ren, b.rect.x, b.rect.y, b.rect.w, b.rect.h, INPUT_BG); draw_text(ren, font, b.text, b.rect.x + 15, b.rect.y + 10, TEXT_WHITE); }
        draw_rect(ren, 20, 250, 260, 40, ACCENT); draw_text(ren, font, "[+] CREATE RESOURCE", 50, 260, TEXT_WHITE);
        draw_text(ren, font_bold, "DYNAMIC EXPLORER", 30, 300, ACCENT);
        draw_rect(ren, 15, 330, 270, 380, EXPLORER_BG);
        for (size_t i = 0; i < explorer_items.size() && i < 9; ++i) {
            draw_rect(ren, 20, 340 + i * 42, 260, 38, INPUT_BG); draw_text(ren, font, explorer_items[i].name, 30, 350 + i * 42, TEXT_WHITE);
        }
        std::string st = (current_state == CONNECTED) ? "ONLINE" : "OFFLINE";
        draw_text(ren, font, "STATUS: " + st, 30, 750, (st == "ONLINE" ? ACCENT : TEXT_DIM));
        draw_rect(ren, 300, 0, 900, 50, SIDEBAR_COLOR); 
        std::string path = "GLOBAL"; for (const auto& u : nav_stack) path += " > " + u.substr(0, 8) + "...";
        draw_text(ren, font_bold, path, 320, 15, ACCENT);
        draw_rect(ren, 320, 70, 860, 630, EXPLORER_BG);
        for (size_t i = 0; i < logs.size(); ++i) draw_text(ren, font, logs[i], 330, 85 + i * 20, TEXT_WHITE);
        draw_rect(ren, 320, 715, 860, 60, INPUT_BG); draw_text(ren, font, "> " + input_text + "|", 340, 735, TEXT_WHITE);

        if (show_create_modal) {
            draw_rect(ren, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MODAL_BG);
            draw_rect(ren, 400, 180, 400, 420, SIDEBAR_COLOR); draw_rect(ren, 400, 180, 400, 420, ACCENT, false);
            draw_text(ren, font_bold, modal_title, 420, 210, ACCENT);
            draw_text(ren, font, field1_name, 420, 260, TEXT_DIM);
            draw_rect(ren, 420, 290, 360, 40, INPUT_BG); if (active_field == 0) draw_rect(ren, 420, 290, 360, 40, ACCENT, false);
            draw_text(ren, font, field1_buf + (active_field == 0 ? "_" : ""), 430, 300, TEXT_WHITE);
            if (field2_name != "(N/A)") {
                draw_text(ren, font, field2_name, 420, 360, TEXT_DIM);
                draw_rect(ren, 420, 390, 360, 40, INPUT_BG); if (active_field == 1) draw_rect(ren, 420, 390, 360, 40, ACCENT, false);
                draw_text(ren, font, field2_buf + (active_field == 1 ? "_" : ""), 430, 400, TEXT_WHITE);
            }
            draw_rect(ren, 420, 520, 160, 50, ACCENT); draw_text(ren, font, "CONFIRM", 465, 535, TEXT_WHITE);
            draw_rect(ren, 620, 520, 160, 50, INPUT_BG); draw_text(ren, font, "CANCEL", 670, 535, TEXT_WHITE);
        } else if (show_item_modal) {
            draw_rect(ren, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MODAL_BG);
            draw_rect(ren, 400, 220, 400, 350, SIDEBAR_COLOR); draw_rect(ren, 400, 220, 400, 350, ACCENT, false);
            draw_text(ren, font_bold, "RESOURCE: " + selected_entity.name.substr(0, 20), 420, 250, ACCENT);
            
            if (selected_entity.type != "REPLY") {
                draw_rect(ren, 420, 300, 360, 45, INPUT_BG);
                draw_text(ren, font, "[>] ENTER / USE", 440, 315, TEXT_WHITE);
            } else {
                draw_rect(ren, 420, 300, 360, 45, {30, 35, 45, 255});
                draw_text(ren, font, "[LOCKED] CANNOT ENTER REPLY", 440, 315, TEXT_DIM);
            }
            
            if (selected_entity.type == "TEAM") {
                draw_rect(ren, 420, 355, 360, 45, INPUT_BG); draw_text(ren, font, "[+] SUBSCRIBE", 440, 370, TEXT_WHITE);
                draw_rect(ren, 420, 410, 360, 45, INPUT_BG); draw_text(ren, font, "[-] UNSUBSCRIBE", 440, 425, TEXT_WHITE);
            } else {
                draw_rect(ren, 420, 355, 360, 45, {30, 35, 45, 255}); draw_text(ren, font, "SUBSCRIPTION NOT APPLICABLE", 440, 370, TEXT_DIM);
            }
            draw_rect(ren, 420, 470, 360, 45, {50, 50, 50, 255}); draw_text(ren, font, "CANCEL", 550, 485, TEXT_DIM);
        }
        SDL_RenderPresent(ren); SDL_Delay(16);
    }
    TTF_Quit(); SDL_Quit(); return 0;
}
