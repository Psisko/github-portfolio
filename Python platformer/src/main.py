""" pygame pro práci s herními elementy,
     player pro práci s hráčem,
      heart pro práci s ukončením hry,
       button pro práci s tlačítky
        world pro práci se světem a
         gameconfig pro základní setup"""
import pygame
from player import Player
from heart import Heart
from button import Button
from world import World
import gameconfig as gs

pygame.init()

# důležité proměnné
Clock = pygame.time.Clock()
start_time = pygame.time.get_ticks()
game_loop = True
show_main_menu = True
show_tutorial_map = False
show_generated_map = False
show_game_over = False
show_you_win = False
# inicializace fontu
font = pygame.font.Font('sprites/cnb.ttf', 70)
# inicializace nového kurzoru
pygame.mouse.set_cursor(*pygame.cursors.diamond)
# inicializace okna
screen = pygame.display.set_mode(gs.WINDOW_SIZE)
# nazev hry
pygame.display.set_caption("2D Plošinovka si piš")
# inicialize herních prvků
player = Player('sprites/idle.png', [2, 4, 2, 1])
tutorial = World('sprites/single_tile.png')
gen_world = World('sprites/single_tile.png')
start_button = Button(950, 50, 250, 100, 'sprites/start.png')
tutorial_button = Button(250, 50, 250, 100, 'sprites/tutorial.png')
gameover_button = Button(50, 150, 750, 300, 'sprites/game_over.png')
win_button = Button(50, 150, 750, 300, 'sprites/you_win.png')
heart = Heart('sprites/full_heart.png', 'sprites/empty_heart.png')
score = 0


def show_map(world):
    """funkce, která vykreslí danou mapu a hráče na obrazovku a vrátí skóre a zda hráč vyhrál/prohrál"""
    map_score = (pygame.time.get_ticks() - start_time) / 1000

    world.draw(screen, player)
    player.update(screen, world)

    for blob in world.blob_list:
        blob.update(screen)

    if heart.update(screen, player):
        world.clean_lists()
        return map_score, False

    if world.winning_flag.colliderect(player.get_rect()):
        world.clean_lists()
        return map_score, True

    return map_score, None


# main game loop
while game_loop:

    screen.fill((150, 200, 255))
    # zobraz hlavní nabídku
    if show_main_menu:
        start_time = pygame.time.get_ticks()
        player.health = 3
        if tutorial_button.draw(screen):
            if tutorial.read_csv('map/tutorial_map.csv'):
                show_main_menu = False
                show_tutorial_map = True
                tutorial.setup_world(player)
            else:
                print('imported map is corrupted')
        elif start_button.draw(screen):
            show_main_menu = False
            show_generated_map = True
            gen_world.generate_map()
            gen_world.setup_world(player)
    # zobraz tutorial mapu
    elif show_tutorial_map:
        score, won = show_map(tutorial)
        if won:
            show_you_win = True
            show_tutorial_map = False
        if won is False:
            show_game_over = True
            show_tutorial_map = False
    # zobraz generovanou mapu
    elif show_generated_map:
        score, won = show_map(gen_world)
        if won:
            show_you_win = True
            show_generated_map = False
        if won is False:
            show_game_over = True
            show_generated_map = False
    # zobraz game over
    elif show_game_over:
        if gameover_button.draw(screen):
            show_main_menu = True
            show_tutorial_map = False
            show_generated_map = False
            show_game_over = False
    # zobraz you win
    elif show_you_win:
        final_score = score + (3 - player.health) * 5
        score_text = font.render(str(final_score), True, (0, 0, 0))
        screen.blit(score_text, pygame.Rect(900, 525, 350, 150))
        if win_button.draw(screen):
            show_main_menu = True
            show_tutorial_map = False
            show_generated_map = False
            show_you_win = False

    pygame.display.update()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            game_loop = False

    Clock.tick(60)

pygame.quit()
