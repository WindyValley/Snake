#include "Resource.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <deque>
using std::deque;
using std::printf;

enum Direction
{
  right,
  down,
  left,
  up
};

class Game
{
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *body_block, *food_block, *button_block;
  TTF_Font *font;
  Mix_Chunk *bk, *eaten;
  SDL_Rect body_src, food_src, button_src;
  SDL_Rect food, button[4];
  SDL_Rect panel, score_panel;
  int score;
  Direction cur_direction;
  deque<SDL_Rect> snake_body;
  deque<Direction> direction_change_deque;

public:
  Game()
      : cur_direction(right), win(nullptr), ren(nullptr), body_block(nullptr),
        food_block(nullptr), button_block(nullptr), font(nullptr),
        eaten(nullptr), bk(nullptr), score(0){};
  ~Game() {}
  bool Init()
  {
    SDL_Surface *tmp;
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
      goto fail;
    win =
        SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         // 1120, 580,
                         640, 580, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!win)
      goto fail;
    ren = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!ren)
      goto fail;

    tmp = SDL_LoadBMP_RW(
        SDL_RWFromMem(const_cast<char *>(res_button.data()), res_button.size()),
        0);
    if (!tmp)
    {
      SDL_LogError(0, "按钮图块加载失败");
      goto fail;
    }
    button_block = SDL_CreateTextureFromSurface(ren, tmp);
    if (!button_block)
      goto fail;
    SDL_FreeSurface(tmp);

    tmp = SDL_LoadBMP_RW(
        SDL_RWFromMem(const_cast<char *>(res_body.data()), res_body.size()), 1);
    if (!tmp)
    {
      SDL_LogError(0, "身体图块加载失败");
      goto fail;
    }
    body_block = SDL_CreateTextureFromSurface(ren, tmp);
    if (!body_block)
      goto fail;
    SDL_FreeSurface(tmp);

    tmp = SDL_LoadBMP_RW(
        SDL_RWFromMem(const_cast<char *>(res_food.data()), res_food.size()), 1);
    if (!tmp)
      goto fail;
    food_block = SDL_CreateTextureFromSurface(ren, tmp);
    if (!food_block)
      goto fail;
    SDL_FreeSurface(tmp);
    button_src = body_src = food_src = {0, 0, 0, 0};
    SDL_QueryTexture(body_block, nullptr, nullptr, &body_src.w, &body_src.h);
    SDL_QueryTexture(food_block, nullptr, nullptr, &food_src.w, &food_src.h);
    SDL_QueryTexture(button_block, nullptr, nullptr, &button_src.w,
                     &button_src.h);

    if (TTF_Init() == -1)
      goto fail;
    font = TTF_OpenFontRW(
        SDL_RWFromMem(const_cast<char *>(res_font.data()), res_font.size()), 1,
        64);
    if (!font)
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "TTF_OpenFont: Open simkai.ttf %s\n", TTF_GetError());
      goto fail;
    }

    if (Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3)
    {
      SDL_Log("MP3解码库加载成功");
    }
    else
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "MP3解码库加载失败");
      goto fail;
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    bk = Mix_LoadWAV_RW(
        SDL_RWFromMem(const_cast<char *>(res_bk.data()), res_bk.size()), 1);
    if (!bk)
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "bk.mp3 load failed!");
      goto fail;
    }
    Mix_VolumeChunk(bk, 30);
    if (Mix_PlayChannel(-1, bk, -1) == -1)
    {
      goto fail;
    }

    eaten = Mix_LoadWAV_RW(
        SDL_RWFromMem(const_cast<char *>(res_tada.data()), res_tada.size()), 1);
    if (!eaten)
    {
      SDL_LogError(0, "tada.wav load failed!");
      goto fail;
    }
    Mix_VolumeChunk(eaten, 100);

    snake_body.push_front({40, 320, 20, 20});
    snake_body.push_front({60, 320, 20, 20});
    snake_body.push_front({80, 320, 20, 20});
    snake_body.push_front({100, 320, 20, 20});
    snake_body.push_front({120, 320, 20, 20});
    srand((unsigned int)time(nullptr));
    food = {120, 280, 20, 20};
    button[0] = {960, 160, 160, 160};
    button[1] = {800, 320, 160, 160};
    button[2] = {640, 160, 160, 160};
    button[3] = {800, 0, 160, 160};

    panel = {0, 480, 640, 100};
    score_panel = {0, 480, 480, 60};

    return true;

  fail:
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "初始化失败");
    UnInit();
    return false;
  }

  void UnInit()
  {
    if (win)
    {
      SDL_DestroyWindow(win);
      win = nullptr;
    }
    if (ren)
    {
      SDL_DestroyRenderer(ren);
      ren = nullptr;
    }
    if (body_block)
    {
      SDL_DestroyTexture(body_block);
      body_block = nullptr;
    }
    if (food_block)
    {
      SDL_DestroyTexture(food_block);
      food_block = nullptr;
    }
    if (button_block)
    {
      SDL_DestroyTexture(button_block);
      button_block = nullptr;
    }
    if (font)
    {
      TTF_CloseFont(font);
      font = nullptr;
    }
    if (Mix_Playing(-1) == SDL_AUDIO_PLAYING)
      Mix_HaltChannel(-1);
    if (eaten)
    {
      Mix_FreeChunk(eaten);
      eaten = nullptr;
    }
    if (bk)
    {
      Mix_FreeChunk(bk);
      bk = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
  }

  void Draw()
  {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_RenderCopy(ren, food_block, &food_src, &food);
    for (const SDL_Rect &r : snake_body)
    {
      SDL_RenderCopy(ren, body_block, &body_src, &r);
    }
    // for (int i = 0; i < 4; ++i)
    // {
    //     SDL_RenderCopyEx(ren, button_block, &button_src, button + i, 90.0 *
    //     i, nullptr, SDL_FLIP_NONE);
    // }

    SDL_SetRenderDrawColor(ren, 200, 180, 20, 255);
    SDL_RenderFillRect(ren, &panel);
    SDL_Rect text_rect{0, 0, 0, 0};
    wchar_t text[] = L"分数: 0000";
    swprintf(text, L"分数: %4d", score);
    SDL_Surface *tmp =
        TTF_RenderUNICODE_Blended(font, (Uint16 *)text, {255, 255, 255, 255});
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, tmp);
    SDL_QueryTexture(tex, nullptr, nullptr, &text_rect.w, &text_rect.h);
    SDL_RenderCopy(ren, tex, &text_rect, &score_panel);
    SDL_FreeSurface(tmp);
    SDL_DestroyTexture(tex);

    SDL_RenderPresent(ren);
  }

  void Move()
  {
    const SDL_Rect &head = snake_body.front();
    SDL_Rect new_node{0, 0, 20, 20};
    if (!direction_change_deque.empty())
    {
      SetDirection(direction_change_deque.front());
      direction_change_deque.pop_front();
    }
    switch (cur_direction)
    {
    case up:
      new_node.x = head.x;
      new_node.y = (head.y + 460) % 480;
      break;
    case down:
      new_node.x = head.x;
      new_node.y = (head.y + 20) % 480;
      break;
    case left:
      new_node.x = (head.x + 620) % 640;
      new_node.y = head.y;
      break;
    case right:
      new_node.x = (head.x + 20) % 640;
      new_node.y = head.y;
      break;
    }
    if (KnockSelf(new_node))
    {
      Reset();
      return;
    }
    snake_body.push_front(new_node);
    if (!SDL_RectEquals(&food, &new_node))
      snake_body.pop_back();
    else
    {
      Mix_PlayChannel(-1, eaten, 0);
      MakeFood();
      ++score;
    }
  }

  void AddDirection(Direction d) { direction_change_deque.push_back(d); }

  void TranslatePoint(SDL_Point p)
  {
    for (int i = 0; i < 4; ++i)
    {
      if (SDL_PointInRect(&p, button + i))
        AddDirection((Direction)i);
    }
  }

private:
  void SetDirection(Direction d)
  {
    if (cur_direction == up || cur_direction == down)
    {
      if (d == left || d == right)
        cur_direction = d;
    }
    else if (d == up || d == down)
      cur_direction = d;
  }

  bool KnockSelf(const SDL_Rect &node)
  {
    for (const SDL_Rect &n : snake_body)
    {
      if (SDL_RectEquals(&n, &node))
        return true;
    }
    return false;
  }

  void Reset()
  {
    snake_body.clear();
    snake_body.push_front({40, 320, 20, 20});
    snake_body.push_front({60, 320, 20, 20});
    snake_body.push_front({80, 320, 20, 20});
    snake_body.push_front({100, 320, 20, 20});
    snake_body.push_front({120, 320, 20, 20});
    srand((unsigned int)time(nullptr));
    cur_direction = right;
    score = 0;
    MakeFood();
  }

  void MakeFood()
  {
    int x = 0, y = 0;
    SDL_Rect tmp;
  create:
    x = rand() % 32 * 20;
    y = rand() % 24 * 20;
    tmp = {x, y, 20, 20};
    for (const SDL_Rect &n : snake_body)
    {
      if (SDL_RectEquals(&n, &tmp))
      {
        goto create;
      }
    }
    food = tmp;
  }
};
