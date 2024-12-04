"""import random pro práci s náhodou, pygame pro práci s herními elementy,
    spritesheet pro práci se sprity a gameconfig pro základní setup"""
import random
import pygame
from spritesheet import Spritesheet
import gameconfig as gc


class Blob:
    """Třida blob spravuje vykreslování bloba a jeho pohyb"""
    def __init__(self, x, y):
        sheet = Spritesheet('sprites/blob.png')
        self.anim_list = [sheet.parse_sheet_animation([4, 4], False),
                          sheet.parse_sheet_animation([4, 4], True)]
        self.coords = [x, y]
        self.spawnpoint = random.randint(x - 50, x + 50)
        self.direction = True
        self.current_action = gc.action["WALK"]
        self.animation_time = pygame.time.get_ticks()
        self.frame = 0

    def get_rect(self):
        """funkce, která vrátí souřadnice a velikost čtverce bloba"""
        return pygame.Rect(self.coords[0] + 16, self.coords[1] + 18, 29, 29)

    def update(self, screen):
        """funkce, která, umožňuje  pohyb přepínání animace bloba"""
        # za každou animation_cooldown dobu změní frame animace
        current_time = pygame.time.get_ticks()
        if current_time - self.animation_time >= gc.animation_cooldown + 50:
            self.frame += 1
            self.animation_time = current_time

        # hlídá, aby se index nekoukal tam, kam nemá (idle i běh mají 4)
        if self.frame >= len(self.anim_list[0][self.current_action]):
            self.frame = 0

        # pohyb bloba
        if self.direction:
            self.coords[0] += 1
        else:
            self.coords[0] -= 1

        # po ujiti 70 pixelu otoci bloba
        if abs(self.spawnpoint - self.coords[0]) > 70:
            self.direction = not self.direction

        # pygame.draw.rect(screen, (255, 255, 255), self.get_rect(), 1)

        if self.direction:
            screen.blit(self.anim_list[0][self.current_action][self.frame], self.coords)
        else:
            screen.blit(self.anim_list[1][self.current_action][self.frame], self.coords)
