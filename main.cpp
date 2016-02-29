#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <string>
using namespace std;

//Screen resolution
const int WIDTH = 1040;
const int HEIGHT = 480;

//variables for events
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *display_event = NULL, *counter_event = NULL, *timer_event;
ALLEGRO_TIMER *timer = NULL, *counter = 0;

//variable used to write the time(minutes and seconds) on the screen with a given font
ALLEGRO_FONT *font = NULL;

//variables that represent the objects that will be drawn on the screen
ALLEGRO_BITMAP *SPIDER=0,*BIN =0,*DIRT=0,*TABLE=0, *CLEANER = 0,*BACKGROUND = 0;

//variable that represents the score
int SCORE=0;


//variables that represent values of CLEANER's speed on x and y
float default_speed = 0.4;
float speed_x = 1.0;
float speed_y = 0.0;

//variables that represent the CLEANER's position(while it's moving).These first values assigned to those variables represent
//the initial position
float clean_x = 0;
float clean_y = 0;

//variables used to represent the table coordinates while it's being positioned by the player
int table_temp_x = 0;
int table_temp_y = 0;

//variable used to represent the amount of tables available for the player
int amount_of_tables = 0;

//variables that represent the time
int minutes = 1;
int seconds = 30;



//variable used to represent which keyboard key is being pressed
int key = 0;

//variable used to close the application if its value is true
bool close_app = false;




//initializing ALLEGRO library and its add-ons
bool initialize()
{

    if (!al_init())
    {
        fprintf(stderr, "Fails to initialize allegro.\n");
        return false;
    }

    al_init_font_addon();
    al_init_image_addon();

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Fails to initialize allegro_ttf.\n");
        return false;
    }

    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Fails to initialize allegro_primitives add-on.\n");
        return false;
    }

    if (!al_install_keyboard())
    {
        fprintf(stderr, "Fails to initialize keyboard.\n");
        return false;
    }



    display = al_create_display(WIDTH,HEIGHT);
    if (!display)
    {
        fprintf(stderr, "Fails to create display.\n");
        return false;
    }

    al_set_window_title(display, "CleaningMyPlace");

    display_event = al_create_event_queue();
    if (!display_event)
    {
        fprintf(stderr, "Fails to create display_event.\n");
        al_destroy_display(display);
        return false;
    }

    timer = al_create_timer(0.1);
    if (!timer)
    {
        fprintf(stderr, "Fails to create timer.\n");
        al_destroy_event_queue(display_event);
        al_destroy_display(display);
        return false;
    }

    font = al_load_font("calibri.ttf", 25, 0);
    if (!font)
    {
        fprintf(stderr, "Fails to load font.\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(display_event);
        al_destroy_display(display);
        return false;
    }

    counter = al_create_timer(1.0);
    if (!counter)
    {
        fprintf(stderr, "Fails to create timer.\n");
        al_destroy_font(font);
        al_destroy_timer(timer);
        al_destroy_event_queue(display_event);
        al_destroy_display(display);
        return false;
    }

    counter_event = al_create_event_queue();
    if (!counter_event)
    {
        fprintf(stderr, "Fails to create counter_event.\n");
        al_destroy_timer(counter);
        al_destroy_font(font);
        al_destroy_timer(timer);
        al_destroy_event_queue(display_event);
        al_destroy_display(display);
        return false;
    }


    al_register_event_source(display_event, al_get_display_event_source(display));
    al_register_event_source(display_event, al_get_timer_event_source(timer));
    al_register_event_source(counter_event, al_get_timer_event_source(counter));
    al_register_event_source(display_event, al_get_keyboard_event_source());


    return true;
}

//this class is used to draw the obstacles on the screen
class Obstacles
{


    public:
    float X,Y;
    ALLEGRO_BITMAP *NAME;
    Obstacles(){}
    Obstacles(ALLEGRO_BITMAP *name, float x, float y) : NAME(name),X(x),Y(y){}

    void draw()
    {
        if(NAME != TABLE)
        al_draw_bitmap(NAME,X,Y,0);

        else
        {
            if(!(X == 0 && Y == 0))
            al_draw_bitmap(NAME,X,Y,0);
        }
    }

};

Obstacles TAB1,TAB2,TAB3;
Obstacles Dirt,Dirt1,Dirt2,Dirt3;
Obstacles SPI;
Obstacles Bin;

//here we analyse the cleaner's colisions on the screen and obstacle's edges
void cleaner_colision(Obstacles NAME)
{

    clean_x+=speed_x;
    clean_y+=speed_y;


    //checking the colision on screen right edge
    if (clean_x >= WIDTH - al_get_bitmap_width(CLEANER))
        {
            speed_x = -default_speed;
            speed_y = 0.0;
            clean_x +=speed_x;

        }

    //checking the colision on screen left edge
    else if (clean_x < 0)
    {
        speed_x = default_speed;
        speed_y = 0.0;
        clean_x += speed_x;
    }

    //checking the colision on screen lower edge
    if (clean_y >= HEIGHT - al_get_bitmap_height(CLEANER))
        {
            speed_y = -default_speed;
            speed_x=0.0;
            clean_y += speed_y;

        }

    //checking the colision on screen top edge
    else if (clean_y < 0)
    {
        speed_y = default_speed;
        speed_x = 0.0;
        clean_y += speed_y;
    }


    //checking the colision on obstacle's right/left edges
    else if ((clean_y >= NAME.Y && clean_y < NAME.Y + al_get_bitmap_height(NAME.NAME))&&
             (clean_x > NAME.X - al_get_bitmap_width(CLEANER) && clean_x <= NAME.X + al_get_bitmap_width(NAME.NAME)) &&
            speed_x !=0)
             {

                 speed_y = default_speed;
                 speed_x = 0.0;
                 clean_y += speed_y;

             }
    else if ((clean_x >= NAME.X && clean_x <= NAME.X + al_get_bitmap_width(NAME.NAME)) &&
             clean_y >= NAME.Y - al_get_bitmap_height(CLEANER) && clean_y <= NAME.Y + al_get_bitmap_height(NAME.NAME) &&
             speed_y != 0)
             {
                 speed_y = 0;
                 speed_x = default_speed;
                 clean_x += speed_x;

             }



    al_draw_bitmap(CLEANER,clean_x,clean_y,0);
}

//this function is used to draw guidelines
void draw_lines(int number_tables)
{

    //number_tables represents the number of tables available in the current level
    if(amount_of_tables < number_tables && minutes>=0 && seconds>0)
    {
        al_draw_bitmap(TABLE,table_temp_x,table_temp_y,0);
            for(int y=0;y<HEIGHT;y+=80)
            {
                al_draw_line(0,y,WIDTH,y,al_map_rgb(0,0,255),2.0);

            }

            for(int x=0;x<WIDTH;x+=80)
            {
                al_draw_line(x,0,x,HEIGHT,al_map_rgb(0,0,255),2.0);

            }
    }
}

//this function is used to draw the tables while they're being positioned by the player
void moving_tables()
{
    if(!al_is_event_queue_empty(display_event))
        {
            //variable used to get events from keyboard
            ALLEGRO_EVENT KEYBOARD;

            //variable used to assign to exit the value of true
            ALLEGRO_EVENT CLOSE_THE_GAME;

            al_wait_for_event(display_event, &KEYBOARD);
            al_wait_for_event(display_event,&CLOSE_THE_GAME);

            if(KEYBOARD.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(KEYBOARD.keyboard.keycode)
                {
                    case ALLEGRO_KEY_UP:
                    key = 1;
                    table_temp_y -=80;
                    break;

                    case ALLEGRO_KEY_DOWN:
                    key = 2;
                    table_temp_y +=80;
                    break;

                    case ALLEGRO_KEY_LEFT:
                    key = 3;
                    table_temp_x -=80;
                    break;

                    case ALLEGRO_KEY_RIGHT:
                    key = 4;
                    table_temp_x +=80;
                    break;

                    case ALLEGRO_KEY_ENTER:
                    amount_of_tables += 1;

                    if (amount_of_tables==1)
                    {
                        TAB1.X = table_temp_x;
                        TAB2.X = table_temp_y;
                    }

                    if(amount_of_tables == 2)
                    {
                        TAB2.X = table_temp_x;
                        TAB2.Y = table_temp_y;
                    }

                    if(amount_of_tables == 3)
                    {
                        TAB3.X = table_temp_x;
                        TAB3.Y = table_temp_y;
                    }

                    table_temp_x = 0;
                    table_temp_y = 0;


                }


            }


            if(CLOSE_THE_GAME.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                close_app = true;
            }
        }
}


//here we make a countdown that represents the remaining time for the player
void calculate_ramaining_time()
{

        if (!al_is_event_queue_empty(counter_event))
        {
            ALLEGRO_EVENT time;
            al_wait_for_event(counter_event, &time);
            if (time.type == ALLEGRO_EVENT_TIMER)
            {
                if(seconds==0 && minutes>=0)
                {
                    seconds=60;
                    minutes--;
                    if(minutes==0)
                    {
                        minutes=0;
                    }
                }
                if (seconds>=0)
                {
                    --seconds;
                    if(seconds==0 && minutes==0)
                    {
                        al_destroy_timer(counter);
                    }
                }
            }

        }

        //showing the time on the screen
        if(seconds>9)
        {
            al_draw_textf(font, al_map_rgb(255, 0, 0), WIDTH-40, 36, ALLEGRO_ALIGN_CENTRE, "%d:%d", minutes, seconds);

        }
        else
        {
            al_draw_textf(font, al_map_rgb(255, 0, 0), WIDTH -40,36, ALLEGRO_ALIGN_CENTRE, "%d:%d%d", minutes,0, seconds);

        }
}

void show_score(Obstacles dirt)
{

    if ((clean_y >= dirt.Y - al_get_bitmap_height(CLEANER) && clean_y <= dirt.Y + al_get_bitmap_height(dirt.NAME))&&
        (clean_x > dirt.X - al_get_bitmap_width(CLEANER) && clean_x <= dirt.X + al_get_bitmap_width(dirt.NAME)))
        {
            SCORE+=10;

        }
    else if((clean_y >= SPI.Y - al_get_bitmap_height(CLEANER) + 5 && clean_y <= SPI.Y + al_get_bitmap_height(SPI.NAME))&&
        (clean_x >= SPI.X - al_get_bitmap_width(CLEANER) && clean_x <= SPI.X + al_get_bitmap_width(SPI.NAME)))
        {
            al_clear_to_color(al_map_rgb(255,255,255));
            al_draw_textf(font, al_map_rgb(255, 0, 0), WIDTH/2,HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "YOU LOST!");
            al_flip_display();
            al_rest(4.0);
            close_app = true;
        }
    else if((clean_y >= Bin.Y - al_get_bitmap_height(CLEANER) && clean_y <= Bin.Y + al_get_bitmap_height(Bin.NAME))&&
        (clean_x > Bin.X - al_get_bitmap_width(CLEANER) && clean_x <= Bin.X + al_get_bitmap_width(Bin.NAME)) && speed_y == default_speed)
       {
           al_clear_to_color(al_map_rgb(255,255,255));
           al_draw_textf(font, al_map_rgb(0, 0, 0), WIDTH/2,HEIGHT/2, ALLEGRO_ALIGN_CENTRE, "SCORE");
           al_draw_textf(font, al_map_rgb(0, 0, 0), WIDTH/2,HEIGHT/2+80, ALLEGRO_ALIGN_CENTRE, "%d",SCORE);
           al_flip_display();
           al_rest(4.0);
           close_app = true;

       }

}


int main()
{
    if (!initialize())
    {
        fprintf(stderr, "Fails to initialize ALLEGRO.\n");
        return -1;
    }



    moving_tables();
    al_clear_to_color(al_map_rgb(255,255,255));

    //loading the objects
    TABLE = al_load_bitmap("TABLE.png");
    SPIDER = al_load_bitmap("SPIDER.png");
    BIN = al_load_bitmap("BIN.png");
    DIRT = al_load_bitmap("DIRT.png");
    CLEANER = al_load_bitmap("CLEANER.png");
    BACKGROUND = al_load_bitmap("BACKGROUND.png");

    al_start_timer(timer);
    al_start_timer(counter);


    Bin.NAME = BIN;Bin.X = WIDTH - 240;Bin.Y = HEIGHT - 80;

    SPI.NAME = SPIDER;SPI.X = WIDTH/2 + 40;SPI.Y = 240;

    Dirt.NAME = Dirt1.NAME = Dirt2.NAME = Dirt3.NAME = DIRT;
    Dirt.X = 160;Dirt.Y = 0;
    Dirt1.X = 320;Dirt1.Y = 160;
    Dirt2.X = 400;Dirt2.Y = 320;
    Dirt3.X = WIDTH - 320;Dirt3.Y = HEIGHT - 160;

    TAB1.NAME = TAB2.NAME = TAB3.NAME = TABLE;

    al_draw_bitmap(BACKGROUND,0,0,0);
    al_flip_display();
    al_rest(3.0);

    while(!close_app)
    {
        moving_tables();

        al_clear_to_color(al_map_rgb(255,255,255));

        //drawing guidelines
        draw_lines(3);


        //drawing images on the screen
        Bin.draw();
        SPI.draw();
        Dirt.draw();
        Dirt1.draw();
        Dirt2.draw();
        Dirt3.draw();



        //drawing tables
        TAB1.draw();
        TAB2.draw();
        TAB3.draw();



        calculate_ramaining_time();


        if(amount_of_tables >= 3 || (minutes==0 && seconds==0))
        {
        //analysing CLEANER'S colisions
        cleaner_colision(TAB1);
        cleaner_colision(TAB2);
        cleaner_colision(TAB3);

        }
        show_score(Dirt);
        show_score(Dirt1);
        show_score(Dirt2);
        show_score(Dirt3);


        al_flip_display();


    }

    al_destroy_timer(timer);
    al_destroy_event_queue(display_event);
    al_destroy_display(display);


}











