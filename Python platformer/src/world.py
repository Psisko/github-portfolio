""" import random pro práci s náhodou,
     csv pro zpracování mapy ve formátu csv,
      os pro správné načtení souboru,
       pygame pro práci s herními elementy,
        numpy pro rychlou práci s 2D poly,
         spritesheet pro práci se sprity,
          blob pro práci s bloby a
           gameconfig pro základní setup"""
import random
import csv
import os
import pygame
import numpy as np
from spritesheet import Spritesheet
from blob import Blob
import gameconfig as gc


def generate_holes(n, terrain):
    """funkce, která vygeneruje do terénu udělá díry a vrátí velikost každé plošiny mezi dírama"""
    same_height_list = []
    temp = 0
    for x in range(6, n - 6):
        if terrain[x] != 10:
            if terrain[x - 1] == terrain[x]:
                temp += 1
            else:
                if same_height_list:
                    temp += 1
                same_height_list.append(temp)
                temp = 0
            if temp == 13:
                same_height_list.append(temp - 3)
                same_height_list.append(3)
                terrain[x - 3: x] = 10
                temp = 0
    return same_height_list


class World:
    """Třida svět spravuje vykreslování celého světa a jeho posouvání,
        svět můžeme nahrát ze souboru nebo ho vygenerovat pomocí algoritmu"""

    def __init__(self, tiles_path):
        self.tile_list = []
        self.blob_list = []
        self.spike_list = []
        self.winning_flag = pygame.Rect(0, 0, 0, 0)
        self.tile_map = []
        self.tile_sheet_list = Spritesheet(tiles_path).parse_tiles(6)
        self.offset = [0, 0, False]

    def clean_lists(self):
        """funkce, která vymaže mapu"""
        self.tile_list = []
        self.blob_list = []
        self.spike_list = []
        self.tile_map = []

    def setup_world(self, player):
        """funkce, která nastaví mapu, tak aby se mohla vykreslit"""
        # najdi start a nastav ho na 6 * TILE_SIZE a 15 * TILE_SIZE
        row_count = 0
        for row in self.tile_map:
            col_count = 0
            for tile in row:
                tile = str(tile)
                if tile == '5':
                    self.offset[0] = col_count * gc.TILE_SIZE - 6 * gc.TILE_SIZE
                    self.offset[1] = row_count * gc.TILE_SIZE - 15 * gc.TILE_SIZE
                    break
                col_count += 1
            row_count += 1

        row_count = 0
        for row in self.tile_map:
            col_count = 0
            for tile in row:
                # tráva
                tile = str(tile)
                if tile == '0':
                    tile = self.create_tile(0, col_count, row_count, True)
                    self.tile_list.append(tile)
                # hlína
                if tile == '1':
                    tile = self.create_tile(1, col_count, row_count, True)
                    self.tile_list.append(tile)
                # bodák
                if tile == '2':
                    tile = self.create_tile(2, col_count, row_count, False)
                    self.tile_list.append(tile)

                    temp = pygame.Rect(tile[1].x + 3, tile[1].y + 23, tile[1].w - 6, tile[1].h - 23)
                    self.spike_list.append(temp)
                # blob
                if tile == '3':
                    new_blob = Blob(col_count * gc.TILE_SIZE - self.offset[0],
                                    row_count * gc.TILE_SIZE - 7 - self.offset[1])
                    self.blob_list.append(new_blob)
                # end flag, tady konci hrac
                if tile == '4':
                    tile = self.create_tile(4, col_count, row_count, False)
                    self.tile_list.append(tile)
                    self.winning_flag = tile[1]
                # start flag, tady zacina hrac
                if tile == '5':
                    tile = self.create_tile(5, col_count, row_count, False)
                    self.tile_list.append(tile)
                    player.coords[0] = tile[1].x - 16
                    player.coords[1] = tile[1].y - 16

                col_count += 1
            row_count += 1

    def read_csv(self, filename):
        """funkce, která umožňuje nahrát mapu ze souboru s příponou .csv"""
        if filename[-4:] != '.csv':
            return False
        tile_map = []
        found_start = False
        found_end = False
        with open(os.path.join(filename)) as data:
            data = csv.reader(data, delimiter=',')
            for row in data:
                for tile in row:
                    if tile == '5':
                        found_start = True
                    if tile == '4':
                        found_end = True
                tile_map.append(row)
        if found_start and found_end:
            self.tile_map = tile_map
        return found_start and found_end

    def draw(self, screen, player):
        """funkce, která vykresluje mapu na obrazovku a při překročení hranice posouvá mapu v daném směru"""
        new_offset = [0, 0]
        self.offset[2] = False

        self.check_window_distance(player, new_offset)

        # kontrola kolize pri posunuti sveta
        if self.offset[2]:
            for tile in self.tile_list:
                temp_tile = pygame.Rect(tile[1].x - new_offset[0], tile[1].y, tile[1].w, tile[1].h)
                if tile[2]:
                    if temp_tile.colliderect(player.get_rect()):
                        new_offset = [0, 0]
                        break

        # vykreslení všech dlaždic a popřípadě jejich posunutí
        for tile in self.tile_list:
            if self.offset[2]:
                tile[1].x -= new_offset[0]
                tile[1].y -= new_offset[1]

            screen.blit(tile[0], tile[1])
            # pygame.draw.rect(screen, (255, 255, 255), tile[1], 1)

        if self.offset[2]:
            # posunuti vsech colideru u spiku
            for spike in self.spike_list:
                spike.x -= new_offset[0]
                spike.y -= new_offset[1]
            # posunuti vsech blobu
            for blob in self.blob_list:
                blob.coords[0] -= new_offset[0]
                blob.coords[1] -= new_offset[1]
                blob.spawnpoint -= new_offset[0]

    def check_window_distance(self, player, new_offset):
        """funkce, která změní offset, pokud se hráč přiblíží ke hranici okna"""
        key = pygame.key.get_pressed()

        # pokud se hrac priblizi k prave strane obrazovky
        if player.coords[0] > gc.WINDOW_SIZE[0] - 500 and key[pygame.K_RIGHT]:
            new_offset[0] = 5
            self.offset[2] = True
        # pokud se hrac priblizi k leve strane obrazovky
        if player.coords[0] < 500 and key[pygame.K_LEFT]:
            new_offset[0] = -5
            self.offset[2] = True

        # pokud se hrac priblizi k horni strane obrazovky
        if player.coords[1] < 200:
            new_offset[1] = -5

        # pokud se hrac priblizi k dolni strane obrazovky
        if player.coords[1] > gc.WINDOW_SIZE[1] - 200:
            new_offset[1] = 5

    def create_tile(self, number_of_sheet, col_count, row_count, collision):
        """funkce, která vytvoří jednotlivou dlaždici"""
        img = pygame.transform.scale(self.tile_sheet_list[number_of_sheet], (gc.TILE_SIZE, gc.TILE_SIZE))
        img_rect = img.get_rect()
        img_rect.x = col_count * gc.TILE_SIZE - self.offset[0]
        img_rect.y = row_count * gc.TILE_SIZE - self.offset[1]
        tile = (img, img_rect, collision, number_of_sheet)
        return tile

    def iteration(self, start, end, scale, roughness, terrain):
        """funkce, která dělá iteraci Diamond-Square alogritmu,
            vezme 2 body, najde jejich prostředek a zvolí mu novou hodnotu,
             poté to samé provádí s nově vzniklými prostředky"""
        half = (end + start) // 2

        if half > 0 and terrain[half] == 0:
            point = (terrain[start] + terrain[end]) // 2
            point += int(random.uniform(-1, 1) * scale)
            if point % 2 == 1:
                point -= 1
            scale *= roughness
            terrain[half] = point

            self.iteration(half, end, scale, roughness, terrain)
            self.iteration(start, half, scale, roughness, terrain)

    def midpoint_displacement(self, n, scale, roughness):
        """funkce, která vygeneruje 1D terén, který je poté použit pro y souřadnice mapy"""
        terrain = np.zeros(n, dtype='i')

        # zvolíme první a poslední bod
        terrain[0] = random.randint(n // 2, (3 * n) // 4)
        if terrain[0] % 2 == 1:
            terrain[0] -= 1
        terrain[n - 1] = terrain[0] + random.randint(-n // 20, n // 20)
        if terrain[n - 1] % 2 == 1:
            terrain[n - 1] -= 1

        # midpoint_displacement algorimtus
        self.iteration(0, n - 1, scale, roughness, terrain)

        return terrain

    def generate_map(self):
        """funkce, která vygeneruje terén pomocí midpoint displacement algoritmu,
            poté ho trošku uhladíme, uděláme do něj díry a rozdělíme ho na úseky se stejnou velikostí
             a nakonec to naházíme do 2D mapy a přidáme náhodně bodáky a bloby"""
        # velikost mapy
        n = 200
        # po jak velikých kouscích měnit souřadnice dalším bodům
        scale = 20
        # drsnost mapy
        roughness = 0.4

        terrain = self.midpoint_displacement(n, scale, roughness)

        # zaplatuj diry velikosti 1
        if terrain[0] != terrain[1]:
            terrain[0] = terrain[1]

        if terrain[-1] != terrain[-2]:
            terrain[-1] = terrain[-2]

        for x in range(1, n - 2):
            if terrain[x - 1] == terrain[x + 1] and terrain[x - 1] != terrain[x]:
                terrain[x] = terrain[x - 1]

        # udelej diry v terenu a rozdel ho na useky se stejnou vyskou
        same_height_list = generate_holes(n, terrain)

        pcg_map = -np.ones((n, n), dtype='i')

        # vyplň mapu plošinama
        x = 0
        for y in terrain:
            # place a spawn point
            if x == 6:
                pcg_map[n - y - 1, x] = 5
            # place a finish point
            if x == n - 6:
                pcg_map[n - y - 1, x] = 4
            pcg_map[n - y, x] = 0
            pcg_map[n - y + 1: n - y + 2, x] = 1
            x += 1

        # vyplň mapu bodáky a bloby
        index = 6
        for height in same_height_list:
            if height >= 7:
                chance = random.randint(1, 5)
                if chance == 1:
                    pcg_map[n - terrain[index] - 1, index + (height // 2) - 1: index + height // 2 + 2] = 2
                if chance == 2:
                    pcg_map[n - terrain[index] - 1, index + height // 2 - 2: index + height // 2 + 1] = 2
                if chance in (3, 4):
                    pcg_map[n - terrain[index] - 1, index + height // 2 - random.randint(0, 1)] = 3
            index += height

        self.tile_map = pcg_map
