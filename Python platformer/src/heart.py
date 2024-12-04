"""import pygame pro práci s herními elementy, spritesheet pro práci se sprity a gameconfig pro základní setup"""
import pygame
from spritesheet import Spritesheet
import gameconfig as gc


class Heart:
    """Třida srdce spravuje vykreslování srdcí na obrazovku a ukončení hry, když hráč umře"""
    def __init__(self, full_heart_sprite_path, empty_heart_sprite_path):
        image = Spritesheet(full_heart_sprite_path).get_sprite(0, 0, 230, 230, False)
        self.full_heart_image = pygame.transform.scale(image, (80, 80))
        image = Spritesheet(empty_heart_sprite_path).get_sprite(0, 0, 230, 230, False)
        self.empty_heart_image = pygame.transform.scale(image, (80, 80))

    def update(self, screen, player):
        """funkce, která vykresluje srdce a vrací boolean podle toho, zda hráč umřel"""
        # pokud hrac nema zivoty nebo spadl z mapy, umira

        if player.health <= 0 or player.coords[1] > gc.WINDOW_SIZE[1]:
            return True

        hearts = player.health - 1
        for temp in range(gc.health):
            if temp > hearts:
                screen.blit(self.empty_heart_image, (temp * 80, 0))
            else:
                screen.blit(self.full_heart_image, (temp * 80, 0))

        return False
