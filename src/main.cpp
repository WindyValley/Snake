#include "Game.hpp"
#include <mutex>
#include <thread>
using std::thread;
namespace chrono = std::chrono;
std::mutex cv_m;

int main(int argc, char **argv)
{
  Game snake;
  bool init_result = false;
  if (!(init_result = snake.Init()))
  {
    return -1;
  }
  bool running = true;
  SDL_Event event{};
  thread move([&]() {
    if (!init_result)
      return;
    SDL_Log("进入状态更新线程");
    chrono::nanoseconds delay_time(200000000);
    while (running)
    {
      auto start = chrono::system_clock::now();
      cv_m.lock();
      snake.Move();
      snake.Draw();
      cv_m.unlock();
      auto end = chrono::system_clock::now();
      chrono::nanoseconds used = end - start;
      std::this_thread::sleep_for(delay_time - used);
    }
  });
  move.detach();
  while (SDL_WaitEvent(&event))
  {
    cv_m.lock();
    switch (event.type)
    {
    case SDL_QUIT:
      running = false;
      SDL_Delay(200);
      goto end;
    case SDL_KEYUP:
    {
      switch (event.key.keysym.sym)
      {
      case SDLK_UP:
      case SDLK_w:
      case SDLK_k:
        snake.AddDirection(up);
        break;
      case SDLK_DOWN:
      case SDLK_s:
      case SDLK_j:
        snake.AddDirection(down);
        break;
      case SDLK_LEFT:
      case SDLK_a:
      case SDLK_h:
        snake.AddDirection(left);
        break;
      case SDLK_RIGHT:
      case SDLK_d:
      case SDLK_l:
        snake.AddDirection(right);
        break;
      default:
        break;
      }
    }
    case SDL_MOUSEBUTTONDOWN:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
        snake.TranslatePoint({event.button.x, event.button.y});
      break;
    }
    default:
      break;
    }
    cv_m.unlock();
  }
end:
  snake.UnInit();
  return 0;
}