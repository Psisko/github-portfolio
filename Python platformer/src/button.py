"""import pygame pro práci s herními elementy a spritesheet pro práci se sprity"""
import pygame
from spritesheet import Spritesheet


class Button:
    """Třida tlačítko spravuje vykreslování tlačítka na obrazovku a reakci na jeho kliknutí"""

    def __init__(self, x, y, w, h, image_path):
        self.image = Spritesheet(image_path).get_sprite(0, 0, w, h, False)
        self.rect = self.image.get_rect()
        self.rect.x = x
        self.rect.y = y

    def draw(self, screen):
        """funkce, která vykresluje tlačítko a vrací, zda bylo tlačítko zmáčknuto"""
        clicked = self.get_clicked()
        screen.blit(self.image, self.rect)
        return clicked

    def get_clicked(self):
        """funkce, která vrátí, zda bylo tlačítko zmáčknuto"""
        if self.rect.collidepoint(pygame.mouse.get_pos()) and pygame.mouse.get_pressed()[0]:
            return True
        return False
