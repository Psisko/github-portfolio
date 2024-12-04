""" import os pro správné načtení souboru,
     pytest pro důležité metody pro testování,
       pygame pro práci s herními elementy,
        heart pro práci s ukončením hry,
         player pro práci s hráčem,
          world pro práci se světem a
           gameconfig pro základní setup"""
import os
import pytest
import pygame
from heart import Heart
from player import Player
from world import World
import gameconfig as gc

os.environ["SDL_VIDEODRIVER"] = "dummy"


def setup_world():
    """funkce správně inicializuje tutorial"""
    pygame.init()
    pygame.display.list_modes()
    screen = pygame.display.set_mode(gc.WINDOW_SIZE)
    player = Player('sprites/idle.png', [2, 4, 2, 1])
    tutorial = World('sprites/single_tile.png')
    tutorial.read_csv('map/tutorial_map.csv')
    tutorial.setup_world(player)
    return screen, player, tutorial


def test_player_action():
    """testovací funkce, která testuje animaci hráče, kterou právě provádí"""
    player = setup_world()[1]
    assert player.current_action == gc.action["IDLE"]
    pygame.quit()


def test_number_of_blobs():
    """testovací funkce, která testuje počet blobů v tutoriálu"""
    tutorial = setup_world()[2]
    assert len(tutorial.blob_list) == 2


def test_player_collide_with_finish_flag():
    """testovací funkce, která testuje colizi hráče s finální vlajkou"""
    player, tutorial = setup_world()[1:]
    player.coords = [tutorial.winning_flag.x, tutorial.winning_flag.y]
    assert player.get_rect().colliderect(tutorial.winning_flag)


@pytest.mark.parametrize(['position_x', 'position_y'], [(7 * gc.TILE_SIZE, 0), (26 * gc.TILE_SIZE, 7 * gc.TILE_SIZE)])
def test_player_collide_with_spikes(position_x, position_y):
    """testovací funkce, která testuje kolizi hráče s bodáky ve 2 různých instancích"""
    player, tutorial = setup_world()[1:]
    pygame.time.wait(1500)
    player.coords[0] += position_x
    player.coords[1] -= position_y
    player.check_enemies_collision(tutorial)

    assert player.health == 2


@pytest.mark.parametrize(['new_coord', 'health'], [(1600, 3), (1200, 2), (901, 1), (900, 0), (800, -1)])
def test_player_out_of_bounds_or_dead(new_coord, health):
    """testovací funkce, která testuje úmrtí hráče ať už na spadnutí z mapy nebo ztracení všech životů,
        funkce se provádí v 5 různých instancích"""
    screen, player, tutorial = setup_world()
    heart = Heart('sprites/full_heart.png', 'sprites/empty_heart.png')

    tutorial.draw(screen, player)
    player.update(screen, tutorial)

    player.coords[1] = new_coord
    player.health = health

    assert heart.update(screen, player) is True


@pytest.mark.parametrize('map_path', ['map/no_start_tutorial_map.csv', 'map/no_finish_tutorial_map.csv'])
def test_csv_map_not_having_starting_and_finishing_flag(map_path):
    """testovací funkce, která testuje, zda má mapa, která se načítá, starovací a koncovou vlajku"""
    pygame.init()
    screen = pygame.display.set_mode(gc.WINDOW_SIZE)
    tutorial = World('sprites/single_tile.png')

    assert tutorial.read_csv(map_path) is False


def test_map_if_not_csv():
    """testovací funkce, která testuje, zda má soubor, který načítáme, koncovku .csv"""
    pygame.init()
    screen = pygame.display.set_mode(gc.WINDOW_SIZE)
    tutorial = World('sprites/single_tile.png')

    assert tutorial.read_csv('map/mapa.tmx') is False
