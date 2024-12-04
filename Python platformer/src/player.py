"""import pygame pro práci s herními elementy, spritesheet pro práci se sprity a gameconfig pro základní setup"""
import pygame
from spritesheet import Spritesheet
import gameconfig as gc


class Player:
    """Třida hráč spravuje vykreslování, jeho pohyb a kolize s prostředím"""

    def __init__(self, sprite_path, sprite_steps):
        sheet = Spritesheet(sprite_path)
        self.anim_list = [sheet.parse_sheet_animation(sprite_steps, False),
                          sheet.parse_sheet_animation(sprite_steps, True)]
        self.coords = [0, 0]
        self.jumped = False
        self.direction = True
        self.current_action = gc.action["IDLE"]
        self.animation_time = pygame.time.get_ticks()
        self.hit_time = self.animation_time
        self.frame = 0
        self.gravity = 0
        self.health = gc.health

    def get_rect(self, dx=0, dy=0):
        """funkce, která vrátí souřadnice a velikost čtverce hráče"""
        return pygame.Rect(self.coords[0] + 19 + dx, self.coords[1] + 15 + dy, 12, 44)

    def update(self, screen, used_world):
        """funkce, která, umožňuje hráči pohyb, přepíná animace a sleduje kolize"""
        new_coords = [0, 0]

        # pokud se svet hybe, hrac se zpomali
        if used_world.offset[2]:
            if self.direction:
                new_coords[0] -= gc.speed
            else:
                new_coords[0] += gc.speed

        # stisknutí klávesy
        self.key_pressed(new_coords)

        # za každou animation_cooldown dobu změní frame animace
        current_time = pygame.time.get_ticks()
        if current_time - self.animation_time >= gc.animation_cooldown:
            self.frame += 1
            self.animation_time = current_time

        # hlídá, aby se index nekoukal tam, kam nemá (běh má 4 frames v animaci a idle má jenom 2)
        if self.frame >= len(self.anim_list[0][self.current_action]):
            self.frame = 0

        # gravitace
        self.gravity += 1
        if self.gravity >= 12:
            self.gravity = 12
        new_coords[1] += self.gravity

        # kolize s pozadím bby
        self.check_world_collision(used_world, new_coords)

        # kolize s neprately
        self.check_enemies_collision(used_world)

        self.coords[0] += new_coords[0]
        self.coords[1] += new_coords[1]

        # pygame.draw.rect(screen, (255, 255, 255), self.get_rect(), 1)

        if self.direction:
            screen.blit(self.anim_list[0][self.current_action][self.frame], self.coords)
        else:
            screen.blit(self.anim_list[1][self.current_action][self.frame], self.coords)

    def print_coords(self):
        """funkce, která vypíše souřadnice"""
        print(self.coords)

    def key_pressed(self, new_coords):
        """funkce, která upraví nové souřadnice a zvolí správný sprite k zobrazení"""
        key = pygame.key.get_pressed()
        if key[pygame.K_RIGHT]:
            new_coords[0] += gc.speed
            self.direction = True
            self.current_action = gc.action["WALK"]
        if key[pygame.K_LEFT]:
            new_coords[0] -= gc.speed
            self.direction = False
            self.current_action = gc.action["WALK"]
        if key[pygame.K_SPACE] and self.jumped is False:
            self.jumped = True
            self.current_action = gc.action["JUMP"]
            self.gravity -= 16
        if key[pygame.K_RIGHT] is False and key[pygame.K_LEFT] is False and self.jumped is False:
            self.current_action = gc.action["IDLE"]
        if key[pygame.K_p]:
            self.print_coords()

    def check_world_collision(self, used_world, new_coords):
        """funkce, která spravuje kolize s podstavami"""
        for tile in used_world.tile_list:
            if tile[2]:
                if tile[1].colliderect(self.get_rect(new_coords[0], 0)):
                    new_coords[0] = 0
                if tile[1].colliderect(self.get_rect(0, new_coords[1])):
                    temp = self.get_rect()
                    if self.gravity < 0:
                        new_coords[1] = tile[1].bottom - temp.top
                        self.gravity = 0
                    elif self.gravity >= 0:
                        new_coords[1] = tile[1].top - temp.bottom
                        self.gravity = 0
                        self.jumped = False

    def check_enemies_collision(self, used_world):
        """funkce, která spravuje kolize s nepřáteli a ubírá hráči životy"""
        current_time = pygame.time.get_ticks()
        for blob in used_world.blob_list:

            # pygame.draw.rect(screen, (255, 255, 255), blob.get_rect(), 1)
            if (self.get_rect().colliderect(blob.get_rect()) and
                    current_time - self.hit_time >= gc.animation_cooldown * 6):
                self.health -= 1
                self.hit_time = current_time

        for spike in used_world.spike_list:
            # pygame.draw.rect(screen, (255, 255, 255), spike, 1)
            if (self.get_rect().colliderect(spike) and
                    current_time - self.hit_time >= gc.animation_cooldown * 6):
                self.health -= 1
                self.hit_time = current_time
