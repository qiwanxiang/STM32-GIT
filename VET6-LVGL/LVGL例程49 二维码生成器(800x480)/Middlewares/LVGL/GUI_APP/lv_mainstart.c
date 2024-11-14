/**
 ****************************************************************************************************
 * @file        lv_mainstart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-03-23
 * @brief       LVGL 二维码生成器 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */
 
#include "LVGL/GUI_APP/lv_mainstart.h"
#include "./BSP/LCD/lcd.h"
#include "lvgl.h"
#include <stdio.h>


LV_FONT_DECLARE(myFont14) \

lv_obj_t *ta;
lv_obj_t * lv_qr_win;
lv_obj_t * lv_qr_cont;
lv_obj_t * lv_qr_btn_win;
lv_obj_t * lv_qr_label;
lv_obj_t * lv_qr_c_label;
const char * data;

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

void lv_qr_windowm(void);

/**
 * @brief  键盘回调
 * @param  event：事件
 * @return 无
 */
void lv_keyboard_cb(lv_event_t *event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)
    {
        if (lv_keyboard_get_selected_btn ((lv_obj_t *)event->user_data) == 39)
        {
            lv_obj_del((lv_obj_t *)event->user_data); /* 删除键盘 */
        }
    }
}

/**
 * @brief  创建键盘
 * @param  无
 * @return 无
 */
static void lv_example_keyboard(void)
{
    lv_obj_t* keyboard = lv_keyboard_create(lv_scr_act());                      /* 定义并创建键盘 */
    lv_keyboard_set_textarea(keyboard, ta);
    lv_obj_add_event_cb(keyboard, lv_keyboard_cb, LV_EVENT_ALL, keyboard);   /* 设置文本框事件回调 */
}

/**
 * @brief  文本区域回调
 * @param  event：事件
 * @return 无
 */
void lv_ta_action_click(lv_event_t *event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)    /* 文本区域点击 */
    {
        lv_example_keyboard();
    }
}

/**
 * @brief  LVGL演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_qr_windowm();
}

/**
 * @brief 退出回调函数
 * @param  event：事件
 * @return 无
 */
static void lv_qr_event_handler(lv_event_t * event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED)    /*点击 */
    {
        lv_obj_del(lv_qr_win);
        lv_mainstart();
    }
}

/**
 * @brief 二维码生成回调函数
 * @param  event：事件
 * @return 无
 */
static void lv_btn_create_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);
    
    if (code == LV_EVENT_CLICKED)
    {
        lv_label_set_text(lv_qr_label,"Generated QR code successfully!");
        
        data = lv_textarea_get_text(ta);
        /* 创建一个lcddev.width/2的二维码 */
        lv_obj_t * qr = lv_qrcode_create(lv_qr_btn_win, lv_obj_get_width(lv_qr_btn_win) - 10, lv_color_hex3(0x33f), lv_color_hex3(0xeef));
        /* 设置数据 */
        lv_qrcode_update(qr, data, strlen(data));
        /* 二维码中间对齐 */
        lv_obj_center(qr);
        lv_obj_invalidate(qr);
    }
}

/**
 * @brief  二维码界面
 * @param  无
 * @return 无
 */
void lv_qr_windowm(void)
{
    
    lv_qr_win = lv_win_create(lv_scr_act(),lv_font_montserrat_24.line_height);
    lv_obj_set_size(lv_qr_win,lv_obj_get_width(lv_scr_act()),lv_obj_get_height(lv_scr_act()));
    lv_obj_set_style_text_font(lv_qr_win,&lv_font_montserrat_14,LV_STATE_DEFAULT);
    lv_win_add_title(lv_qr_win, "QR V1.0");
    lv_obj_t * btn = lv_win_add_btn(lv_qr_win, LV_SYMBOL_CLOSE, 50);
    lv_obj_add_event_cb(btn, lv_qr_event_handler, LV_EVENT_CLICKED, NULL);
    
    lv_qr_cont = lv_win_get_content(lv_qr_win);
    
    ta = lv_textarea_create(lv_qr_cont);                           /* 创建文本区域控件 */
    lv_obj_set_style_radius(ta, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(ta,&myFont14,LV_STATE_DEFAULT);     /* 设置文本字体 */
    lv_obj_set_style_border_color(ta,lv_color_make(0,0,0),LV_STATE_DEFAULT);
    lv_obj_set_size(ta, lv_obj_get_width(lv_scr_act())/2, lv_obj_get_height(lv_scr_act())/2);    /* 文本区域的大小 */
    lv_obj_align_to(ta, lv_qr_cont, LV_ALIGN_TOP_LEFT, 0, 30);     /* 对齐 */
    lv_textarea_set_cursor_click_pos(ta, true);                    /* 单击文本区域上的文本启用/禁用光标定位 */
    lv_obj_add_event_cb(ta, lv_ta_action_click,LV_EVENT_ALL,NULL); /* 设置回调函数 */
    lv_textarea_set_text(ta, " ");                                 /* 设置初始状态的文本为空 */
    
    lv_qr_btn_win = lv_obj_create(lv_qr_cont);
    lv_obj_set_style_border_color(lv_qr_btn_win,lv_color_make(0,0,0),LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lv_qr_btn_win, 0, LV_PART_MAIN);
    lv_obj_set_size(lv_qr_btn_win,lv_obj_get_width(ta)/2 + 40,lv_obj_get_width(ta)/2 + 40);
    lv_obj_invalidate(lv_qr_btn_win);
    lv_obj_align_to(lv_qr_btn_win, ta, LV_ALIGN_OUT_RIGHT_MID, lv_obj_get_width(ta)/4, 0);     /* 对齐 */
    
    lv_obj_t *lv_qr_btn = lv_btn_create(lv_qr_cont);
    lv_obj_set_style_bg_color(lv_qr_btn,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    lv_obj_align_to(lv_qr_btn, lv_qr_btn_win, LV_ALIGN_OUT_BOTTOM_LEFT, 0, lv_obj_get_height(lv_qr_btn_win)/6);     /* 对齐 */
    lv_obj_set_size(lv_qr_btn,lv_obj_get_width(lv_qr_btn_win),lv_obj_get_height(lv_qr_btn_win)/3);
    lv_obj_add_event_cb(lv_qr_btn,lv_btn_create_cb,LV_EVENT_ALL,NULL);
    
    lv_obj_t *lv_qr_btn_label = lv_label_create(lv_qr_btn);
    lv_obj_set_style_text_color(lv_qr_btn_label,lv_color_make(255,255,255),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lv_qr_btn_label,&myFont14,LV_STATE_DEFAULT);     /* 设置文本字体 */
    lv_obj_clear_flag(lv_qr_btn_win,LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_text(lv_qr_btn_label,"Create Qr");
    lv_obj_center(lv_qr_btn_label);
    
    lv_qr_label = lv_label_create(lv_qr_cont);
    lv_obj_set_style_text_color(lv_qr_label,lv_palette_main(LV_PALETTE_BLUE),LV_STATE_DEFAULT);
    lv_label_set_text(lv_qr_label,"");
    lv_obj_align_to(lv_qr_label, ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, lv_obj_get_height(ta)/6);            /* 对齐 */
    
    lv_qr_c_label = lv_label_create(lv_qr_cont);
    lv_obj_set_style_text_color(lv_qr_c_label,lv_palette_main(LV_PALETTE_GREY),LV_STATE_DEFAULT);
    lv_label_set_text(lv_qr_c_label,"Alientek:http://www.openedv.com/");
    lv_obj_align_to(lv_qr_c_label, lv_qr_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, lv_obj_get_height(ta)/6); /* 对齐 */
}
