#include "snake.h"
#include "screen.h"
#include <time.h>
#include<math.h>
#include <unistd.h>
#define start_speed 300
CSnake::CSnake(CRect r, char _c /*=' '*/):      //duzy czas odswiezania
CFramedWindow(r, _c)
{
    loop = 0;
    score = 0;
    h_bool=true;
    p_bool=false;
    is_moving = false;
    gm_ov_bool = false;
    is_fruit_bool=false;
    speed = start_speed;
    timeout(start_speed);
    fruit_position.y = 5;
    fruit_position.x = 20;
    snake_body.dir = RIGHT;
    snake_body.part.push_back(CPoint(4,2));
    snake_body.part.push_back(CPoint(3,2));
    snake_body.part.push_back(CPoint(2,2));
}
void CSnake::paint()
{
        CFramedWindow::paint();
        fruit();
        snake();
        if(h_bool)
            help();
        level();
}
void CSnake::help() // void CInputLine::paint() tu mozna było uzyc
{
    char options[5][40] = {"h - toggle help information","p - toggle pause/play mode","r - restart game",
    "arrows - move snake (in play mode) or","move window (in pause mode)"};

        for(int i = 0; i < 5;i++){
            gotoyx( geom.topleft.y + 4 + i,geom.topleft.x + 3);
            printl(options[i]);
        }
}
bool CSnake::handleEvent(int key)
{
    if(!h_bool && !p_bool)
    {
      is_moving = true;
      switch (key)
      {
        case KEY_UP:
          if(snake_body.dir != DOWN)
          snake_body.dir = UP;
          break;
        case KEY_DOWN:
          if(snake_body.dir != UP)
          snake_body.dir = DOWN;
          break;
        case KEY_RIGHT:
          if(snake_body.dir != LEFT)
          snake_body.dir = RIGHT;
          break;
        case KEY_LEFT:
          if(snake_body.dir != RIGHT)
          snake_body.dir = LEFT;
          break;
      }
    }
  if(key == 'h' || key == 'H')
  {
    h_bool = !h_bool;
    if(h_bool == true)
    is_moving = false;
    if(h_bool == false)
    is_moving = true;
    return true;
  }
  else if(key == 'p' || key == 'P') // pause mode
  {
        p_bool = !p_bool;
        if(p_bool == true)
        is_moving = false;
        if(p_bool == false)
        is_moving = true;
        return true;
  }
  else if(key == 'r' || key == 'R')
  {
        restart();
        return true;
  }
  else if(is_moving == false )
  {
       if(CWindow::handleEvent(key))
           return true;
       return false;
  }
  step();
  //usleep(100000);
  //false jesli nie am zadnego przycisku oraz jest h_bool lub p_bull
  return true;
}
void CSnake::snake()
{
    if(gm_ov_bool){
        gotoyx(geom.topleft.y+1,geom.topleft.x+(geom.size.x/2)-1);
        printl("GAME OVER");
    }
    /*init_pair(1, COLOR_BLACK,COLOR_GREEN);//waz tlo
	attron(COLOR_PAIR(1));*/
    for(list< CPoint >::iterator i = snake_body.part.begin(); i != snake_body.part.end(); i++)//ogon
    {
        gotoyx(i->y+geom.topleft.y,i->x+geom.topleft.x);
        printc('+');
    }
    gotoyx(snake_body.part.begin()->y+geom.topleft.y,snake_body.part.begin()->x+geom.topleft.x);//glowa
    printc('*');
    //attroff(COLOR_PAIR(1)); //off color
}
void CSnake::step()
{
    if(!h_bool && !p_bool && !gm_ov_bool)//chodzi jesli menu wylaczone
    {

                snake_body.part.push_front(snake_body.part.front());
                switch(snake_body.dir)
                {
                    case UP:
                        snake_body.part.front() +=  CPoint(0, -1);
                        if(snake_body.part.front().y == 0)
                            snake_body.part.front().y = geom.size.y - 2;
                        break;
                    case RIGHT:
                        snake_body.part.front() +=  CPoint(1, 0);
                        if(snake_body.part.front().x == geom.size.x - 1)
                            snake_body.part.front().x = 1;
                        break;
                    case DOWN:
                        snake_body.part.front() +=  CPoint(0, 1);
                        if(snake_body.part.front().y == geom.size.y - 1)
                            snake_body.part.front().y = 1;
                        break;
                    case LEFT:
                        snake_body.part.front() +=  CPoint(-1, 0);
                        if(snake_body.part.front().x == 0)
                            snake_body.part.front().x = geom.size.x - 2;
                        break;
                }
                bite();
                CPoint temp = snake_body.part.front();
                snake_body.part.pop_front();
                if(collision(temp))
                    gm_ov_bool = true;
                snake_body.part.push_front(temp);
    }
}
void CSnake::restart()
{
    score = 0;
    h_bool = true;
    p_bool = false;
    is_moving = false;
    gm_ov_bool = false;
    speed = start_speed;
    timeout(start_speed);
    is_fruit_bool = false;
    snake_body.dir = RIGHT;
    snake_body.part.clear();
    snake_body.part.push_back(CPoint(4,2));
    snake_body.part.push_back(CPoint(3,2));
    snake_body.part.push_back(CPoint(2,2));
}
void CSnake::fruit()
{
    if(!is_fruit_bool)
    {
        srand((int) time(0));
        do{
            fruit_position.x = ( std::rand() % (geom.size.x-3))+1;
            fruit_position.y = ( std::rand() % (geom.size.y-3))+1;

        }while(collision(fruit_position)); //Jeżeli owoc będzie na wężu to losuje kolejny raz
        is_fruit_bool = true;
    }

    gotoyx(fruit_position.y+geom.topleft.y,fruit_position.x+geom.topleft.x);
    printc('O');
}
bool CSnake::collision(CPoint &it)
{
    for(list< CPoint >::iterator i = snake_body.part.begin(); i != snake_body.part.end(); i++)//ogon
        if(i->x == it.x && i->y == it.y)
            return true;
    return false;
}
void CSnake::bite()
{
    if( (snake_body.part.front().x == fruit_position.x) && (snake_body.part.front().y == fruit_position.y))
    {
        is_fruit_bool = false;
        score+=1;
        if(speed > 10)
        speed -= 9;
        timeout(speed);
    }
    else
        snake_body.part.pop_back();


}
void CSnake::level()
{
    gotoyx(geom.topleft.y-1,geom.topleft.x);
    printl("| SCORE : %d |",score);

}

