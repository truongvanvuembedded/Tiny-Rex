//==================================================================================================
//	File Name	: obstacles_object.cpp
//	CPU Type	: STM32L151CBT6A
//	Builder		: gcc-arm-none-eabi
//	Coding		: V.Vu
//	History		: V.Vu 08/23/2026 First version
//	Outline		: The Obstacle object manages obstacle spawning, movement, spacing, and collision detection with the T-Rex.
//==================================================================================================
//==================================================================================================
//	#pragma section
//==================================================================================================

//==================================================================================================
//	Local Compile Option
//==================================================================================================

//==================================================================================================
//	Header File
//==================================================================================================
#include "obstacles_object.h"
#include "tsm.h"
//==================================================================================================
//	Local define
//==================================================================================================
#define OBSTAJCLE_MAX (5)
/* X Axis position when object go all the way cross the screen */
#define AXIS_X_OBSTACLE_OBJECT_CROSS_SCREEN(object) (0 - g_bitmap_table[object].width)
/* Y Axis position */
#define AXIS_Y_OBSTACLE_OBJECT_IN_GROUND(object) (HEIGHT - g_bitmap_table[object].height)
/* Axis */
#define AXIS_X_OBSTACLE_OBJECT_INIT (WIDTH)
#define AXIS_Y_OBSTACLE_OBJECT_INIT(object) AXIS_Y_OBSTACLE_OBJECT_IN_GROUND(object)
/* Speed */
#define SPEED_MIN (3)
#define SPEED_MAX (8)
/* Distance for create new object */
#define SPAWN_DISTANCE_MIN (WIDTH / 2)
#define SPAWN_DISTANCE_MAX (WIDTH)
/* Y position random for bird */
#define AXIS_Y_BIRD_OBJECT_MIN (5)
#define AXIS_Y_BIRD_OBJECT_MAX (HEIGHT - 20)
//==================================================================================================
//	Local define I/O
//==================================================================================================

//==================================================================================================
//	Local Struct Template
//==================================================================================================

//==================================================================================================
//	Local RAM
//==================================================================================================

//==================================================================================================
//	Global RAM
//==================================================================================================
static game_object_t obstacle_objects[OBSTAJCLE_MAX];
static uint8_t min_spawn_distance_meter; /* Minimum distance to create a new obstacle */
static int8_t cur_dis_meter_last_obj;    /* Current distance meter of last object created */
static uint8_t id_last_obj;              /* IDof last object created */
uint8_t obstacle_objects_speed;
//==================================================================================================
//	Local ROM
//==================================================================================================
static void obstacle_objects_update(void);
static void collision_detect(void);
/* TSM action */
static void obstacle_on_play(ak_msg_t* msg);
static void obstacle_on_move(ak_msg_t* msg);
static void obstacle_on_check_collision(ak_msg_t* msg);
static void obstacle_on_inc_speed(ak_msg_t* msg);
static bool collision_check(const game_object_t* obj1,
                            const bitmap_info_t* bmp1,
                            const game_object_t* obj2,
                            const bitmap_info_t* bmp2);
//==================================================================================================
//	Local Function Prototype
//==================================================================================================

//==================================================================================================
//	State Table (TSM)
//	RUNNING : obstacles move and spawn.
//	COLLIDED: T-Rex hit an obstacle, obstacles stop moving (MOVE is not in the table).
//==================================================================================================
typedef enum
{
    EM_OBSTACLE_STATE_RUNNING = 0,
    EM_OBSTACLE_STATE_COLLIDED,
} EM_OBSTACLE_STATE;

static tsm_t obstacle_tsm_running[] =
    {
        {OBSTACLE_PLAY_EVENT, EM_OBSTACLE_STATE_RUNNING, obstacle_on_play},
        {OBSTACLE_MOVE_EVENT, TSM_NULL_STATE, obstacle_on_move},
        {OBSTACLE_CHECK_COLLISSION_EVENT, TSM_NULL_STATE, obstacle_on_check_collision},
        {OBSTACLE_INC_SPEED_EVENT, TSM_NULL_STATE, obstacle_on_inc_speed},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

static tsm_t obstacle_tsm_collided[] =
    {
        {OBSTACLE_PLAY_EVENT, EM_OBSTACLE_STATE_RUNNING, obstacle_on_play},
        {OBSTACLE_CHECK_COLLISSION_EVENT, TSM_NULL_STATE, obstacle_on_check_collision},
        {OBSTACLE_INC_SPEED_EVENT, TSM_NULL_STATE, obstacle_on_inc_speed},
        {TSM_NULL_MSG, TSM_NULL_STATE, TSM_NULL_ROUTINE},
};

/* Index of this table MUST be the same as EM_OBSTACLE_STATE */
static tsm_t* obstacle_tsm_table[] =
    {
        obstacle_tsm_running,
        obstacle_tsm_collided,
};

static tsm_tbl_t obstacle_tsm =
    {
        EM_OBSTACLE_STATE_RUNNING,
        TSM_NULL_ON_STATE,
        obstacle_tsm_table,
};
//==================================================================================================
//	Source Code
//==================================================================================================
void obstacle_objects_handle(ak_msg_t* msg)
{
    tsm_dispatch(&obstacle_tsm, msg);
}

static void obstacle_on_play(ak_msg_t* msg)
{
    (void)msg;
    for (uint8_t au1_ForC = 0; au1_ForC < OBSTAJCLE_MAX; au1_ForC++)
    {
        obstacle_objects[au1_ForC].visible = BLACK;
    }
    obstacle_objects_speed = SPEED_MIN;
    min_spawn_distance_meter = SPAWN_DISTANCE_MAX;
    cur_dis_meter_last_obj = 0;
    id_last_obj = 0;
    /* Create first object */
    obstacle_objects[0].visible = WHITE;
    obstacle_objects[0].speed = obstacle_objects_speed;
    obstacle_objects[0].action_image = BITMAP_TREE_1;
    obstacle_objects[0].x = AXIS_X_OBSTACLE_OBJECT_INIT;
    obstacle_objects[0].y = AXIS_Y_OBSTACLE_OBJECT_INIT(obstacle_objects[0].action_image);
}

static void obstacle_on_move(ak_msg_t* msg)
{
    (void)msg;
    obstacle_objects_update();
}

static void obstacle_on_check_collision(ak_msg_t* msg)
{
    (void)msg;
    collision_detect();
}

static void obstacle_on_inc_speed(ak_msg_t* msg)
{
    (void)msg;
    if (obstacle_objects_speed < SPEED_MAX)
    {
        obstacle_objects_speed++;
    }
    else
    {
        min_spawn_distance_meter -= 10;
        if (min_spawn_distance_meter < SPAWN_DISTANCE_MIN)
        {
            min_spawn_distance_meter = SPAWN_DISTANCE_MIN;
        }
    }
}
void draw_obstacle_objects(void)
{
    for (uint8_t au1_ForC = 0; au1_ForC < OBSTAJCLE_MAX; au1_ForC++)
    {
        if (obstacle_objects[au1_ForC].visible == WHITE)
        {
            /* Clear image before write for avoid line ground over write to object */
            view_render.fillRoundRect(
                obstacle_objects[au1_ForC].x,
                obstacle_objects[au1_ForC].y,
                g_bitmap_table[obstacle_objects[au1_ForC].action_image].width,
                g_bitmap_table[obstacle_objects[au1_ForC].action_image].height,
                0,
                BLACK);
            // Draw bit-map of bird
            view_render.drawBitmap(
                obstacle_objects[au1_ForC].x,
                obstacle_objects[au1_ForC].y,
                g_bitmap_table[obstacle_objects[au1_ForC].action_image].bitmap,
                g_bitmap_table[obstacle_objects[au1_ForC].action_image].width,
                g_bitmap_table[obstacle_objects[au1_ForC].action_image].height,
                obstacle_objects[au1_ForC].visible);
        }
    }
}
static void obstacle_objects_update(void)
{
    /* Upte position of each objects */
    for (uint8_t au1_ForC = 0; au1_ForC < OBSTAJCLE_MAX; au1_ForC++)
    {
        if (obstacle_objects[au1_ForC].visible == WHITE)
        {
            obstacle_objects[au1_ForC].x -= obstacle_objects[au1_ForC].speed;
            /* Remove object when it move complete screen */
            if (obstacle_objects[au1_ForC].x <= AXIS_X_OBSTACLE_OBJECT_CROSS_SCREEN(obstacle_objects[au1_ForC].action_image))
            {
                obstacle_objects[au1_ForC].visible = BLACK;
            }
            /* Update animation for bird */
            if (obstacle_objects[au1_ForC].action_image == BITMAP_BIRD_1)
            {
                obstacle_objects[au1_ForC].action_image = BITMAP_BIRD_2;
            }
            else if (obstacle_objects[au1_ForC].action_image == BITMAP_BIRD_2)
            {
                obstacle_objects[au1_ForC].action_image = BITMAP_BIRD_1;
            }
        }
    }
    /* Udpate distance meter or last object */
    cur_dis_meter_last_obj = obstacle_objects[id_last_obj].x;
    /* Create new object when last object move corss distance */
    if (cur_dis_meter_last_obj < (WIDTH - min_spawn_distance_meter))
    {
        id_last_obj++;
        if (id_last_obj >= OBSTAJCLE_MAX)
        {
            id_last_obj = 0;
        }
        obstacle_objects[id_last_obj].visible = WHITE;
        obstacle_objects[id_last_obj].speed = obstacle_objects_speed;
        obstacle_objects[id_last_obj].action_image = random(BITMAP_BIRD_1, BITMAP_TREE_4 + 1);
        obstacle_objects[id_last_obj].x = AXIS_X_OBSTACLE_OBJECT_INIT;
        if (obstacle_objects[id_last_obj].action_image == BITMAP_BIRD_1 || obstacle_objects[id_last_obj].action_image == BITMAP_BIRD_2)
        {
            /* Limit Y position of bird object */
            obstacle_objects[id_last_obj].y = random(AXIS_Y_BIRD_OBJECT_MIN, AXIS_Y_BIRD_OBJECT_MAX);
        }
        else
        {
            obstacle_objects[id_last_obj].y = AXIS_Y_OBSTACLE_OBJECT_INIT(obstacle_objects[id_last_obj].action_image);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : collision_check
//	Function : Check collision between objects
//	Argument : const game_object_t *obj1: Information of object 1
//             const bitmap_info_t *bmp1: Bitmap of object 1
//             const game_object_t *obj2: Information of object 2
//             const bitmap_info_t *bmp2: Bitmap of object 2
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static bool collision_check(const game_object_t* obj1,
                            const bitmap_info_t* bmp1,
                            const game_object_t* obj2,
                            const bitmap_info_t* bmp2)
{
    if ((!obj1->visible) || (!obj2->visible))
    {
        return false;
    }

    return ((obj1->x + bmp1->width > obj2->x) &&
            (obj1->y + bmp1->height > obj2->y) &&
            (obj2->x + bmp2->width - 2 > obj1->x) &&
            (obj2->y + bmp2->height - 2 > obj1->y));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Name     : collision_detect
//	Function : Detect collistion between tiny rex with other objects
//	Argument : None
//	Return   : None
//	Created  : 13/07/2026 V.Vu
//	Changed  : -
//	Remarks  : -
////////////////////////////////////////////////////////////////////////////////////////////////////
static void collision_detect(void)
{
    bool evaluated = false;
    bool collision = false;

    /* Upte position of each objects */
    for (uint8_t au1_ForC = 0; au1_ForC < OBSTAJCLE_MAX; au1_ForC++)
    {
        if (obstacle_objects[au1_ForC].visible == WHITE)
        {
            evaluated = true;
            collision = collision_check(&tiny_rex_object,
                                        &g_bitmap_table[tiny_rex_object.action_image],
                                        &obstacle_objects[au1_ForC],
                                        &g_bitmap_table[obstacle_objects[au1_ForC].action_image]);
            if (collision)
            {
                task_post_pure_msg(TINY_REX_TASK_DISPLAY_ID, TINY_REX_DISPLAY_PLAYING_GAME_OVER);
            }
        }
    }
    /* State follows the result of the last checked obstacle */
    if (evaluated)
    {
        TSM_TRAN(&obstacle_tsm, collision ? EM_OBSTACLE_STATE_COLLIDED : EM_OBSTACLE_STATE_RUNNING);
    }
}