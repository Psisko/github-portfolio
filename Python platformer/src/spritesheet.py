"""import os pro správné načtení souboru, pygame pro práci s herními elementy a gameconfig pro základní setup"""
import os
import pygame
import gameconfig as gc


class Spritesheet:
    """Třida Spritesheet umožňuje nahrát sprity, zpracovat je a správně vykreslit"""
    def __init__(self, filename):
        self.sprite_sheet = pygame.image.load(os.path.join(filename)).convert_alpha()

    def get_sprite(self, x, y, w, h, rotate_left):
        """funkce vrátí sprite s velikostí 2x(w,h) na místo x,y , sprite můžeme i orotovat podle osy x"""
        surface = pygame.Surface((w, h))
        surface.set_colorkey(gc.IMPORT_BACKGROUND)
        surface.blit(self.sprite_sheet, (0, 0), (x, y, w, h))
        # 2x zvětšíme
        sprite = pygame.transform.scale(surface, (w * 2, h * 2))
        if rotate_left:
            return pygame.transform.flip(sprite, True, False)
        return sprite

    def parse_sprite(self, x, rotate_left=False):
        """funkce vrátí pouze sprite na souřadnicích (x * TILE_SIZE, 0)"""
        sprite_part = self.get_sprite(x * gc.TILE_SIZE, 0, gc.TILE_SIZE, gc.TILE_SIZE, rotate_left)
        return sprite_part

    def parse_sheet_animation(self, anim_list_steps, rotate_left):
        """funkce dostane obrázek se všemi sprity a nahraje je podle anim_list_steps do jednotlivých listů"""
        different_anim = 0
        anim_list = []
        for anim_steps in anim_list_steps:
            temp_list = []
            for _ in range(anim_steps):
                temp_list.append(self.parse_sprite(different_anim, rotate_left))
                different_anim += 1
            anim_list.append(temp_list)
        return anim_list

    def parse_tiles(self, number_of_tiles):
        """funkce dostane počet dlaždic a každou nahraje do listu"""
        tiles_list = []
        for x in range(number_of_tiles):
            tiles_list.append(self.parse_sprite(x))
        return tiles_list
