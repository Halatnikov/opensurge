/*
 * Open Surge Engine
 * credits.c - credits scene
 * Copyright (C) 2009-2012, 2019  Alexandre Martins <alemartf@gmail.com>
 * http://opensurge2d.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include "credits.h"
#include "options.h"
#include "../core/util.h"
#include "../core/fadefx.h"
#include "../core/color.h"
#include "../core/video.h"
#include "../core/audio.h"
#include "../core/lang.h"
#include "../core/input.h"
#include "../core/timer.h"
#include "../core/scene.h"
#include "../core/storyboard.h"
#include "../core/font.h"
#include "../core/assetfs.h"
#include "../core/logfile.h"
#include "../entities/actor.h"
#include "../entities/background.h"
#include "../entities/sfx.h"



/* private data */
static const char* CREDITS_BGFILE = "themes/scenes/credits.bg";
extern const char CREDITS_TEXT[];
static const int scroll_speed = 30;
static int quit;
static font_t *title, *text, *back;
static input_t *input;
static bgtheme_t *bgtheme;
static music_t *music;
static scene_t *next_scene;



/* public functions */

/*
 * credits_init()
 * Initializes the scene
 */
void credits_init(void *foo)
{
    quit = FALSE;
    next_scene = NULL;
    input = input_create_user(NULL);
    music = music_load(OPTIONS_MUSICFILE);

    title = font_create("MenuTitle");
    font_set_text(title, "%s", lang_get("CREDITS_TITLE"));
    font_set_position(title, v2d_new(VIDEO_SCREEN_W/2, 5));
    font_set_align(title, FONTALIGN_CENTER);

    back = font_create("MenuText");
    font_set_text(back, "%s", lang_get("CREDITS_BACK"));
    font_set_position(back, v2d_new(10, VIDEO_SCREEN_H - font_get_textsize(back).y - 5));

    text = font_create("MenuText");
    font_set_text(text, "%s", CREDITS_TEXT);
    font_set_width(text, VIDEO_SCREEN_W - 20);
    font_set_position(text, v2d_new(10, VIDEO_SCREEN_H));

    bgtheme = background_load(CREDITS_BGFILE);

    fadefx_in(color_rgb(0,0,0), 1.0);
}


/*
 * credits_release()
 * Releases the scene
 */
void credits_release()
{
    bgtheme = background_unload(bgtheme);

    font_destroy(title);
    font_destroy(text);
    font_destroy(back);

    input_destroy(input);
    music_unref(music);
}


/*
 * credits_update()
 * Updates the scene
 */
void credits_update()
{
    float scroll_speed_multiplier = 1.0f;
    float dt = timer_get_delta();
    v2d_t textpos;

    /* background movement */
    background_update(bgtheme);

    /* scroll the text faster */
    if(input_button_down(input, IB_FIRE1))
        scroll_speed_multiplier = 5.0f;

    /* text movement */
    textpos = font_get_position(text);
    textpos.y -= scroll_speed_multiplier * scroll_speed * dt;
    if(textpos.y < -font_get_textsize(text).y)
        textpos.y = VIDEO_SCREEN_H;
    font_set_position(text, textpos);

    /* quit */
    if(!quit && !fadefx_is_fading()) {
        if(input_button_pressed(input, IB_FIRE4)) {
            sound_play(SFX_BACK);
            next_scene = NULL;
            quit = TRUE;
        }
    }

    /* music */
    if(!music_is_playing())
        music_play(music, true);

    /* fade-out */
    if(quit) {
        if(fadefx_is_over()) {
            scenestack_pop();
            if(next_scene != NULL)
                scenestack_push(next_scene, NULL);
            return;
        }
        fadefx_out(color_rgb(0,0,0), 1.0);
    }
}


/*
 * credits_render()
 * Renders the scene
 */
void credits_render()
{
    v2d_t cam = v2d_new(VIDEO_SCREEN_W/2, VIDEO_SCREEN_H/2);

    background_render_bg(bgtheme, cam);
    font_render(text, cam);
    background_render_fg(bgtheme, cam);
    font_render(title, cam);
    font_render(back, cam);
}
