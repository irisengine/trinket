#!/usr/bin/env python3

import yaml

map_str = '''
..p............
.###O..........
.#e#...........
.##########....
...#......#....
...#.....#####.
...###########.
........###e##.
........####e#.
........#e####.
...............
'''

map_str = map_str.strip()

zone = {
    'player_start_position': [0.0, 0.0, 0.0],
    'name': 'dungeon',
    'static_geometry': [{
        'position': [0.0, -1002.0, 0.0],
        'orientation': [0.0, 0.0, 0.0],
        'scale': [1000.0, 1000.0, 1000.0],
        'mesh_type': 'cube',
        'texture': 'grass.png',
        'rigid_body': True,
        'rigid_body_type': 'bounding_box'
    }],
    'enemies': []
}

lines = map_str.split('\n')
assert all([len(line) == len(lines[0]) for line in lines])

floor_codes = ['#', 'p', 'e', 'O']

for y in range(len(lines)):
    for x in range(len(lines[y])):
        cell_code = lines[y][x]

        if cell_code == '.':
            continue

        if cell_code in floor_codes:
            zone['static_geometry'].append({
                'position': [x * 10.0, -2.4, y * 10.0],
                'orientation': [0.0, 0.0, -1.570796327],
                'scale': [5.0, 5.0, 5.0],
                'mesh_type': 'Floor_Modular.fbx',
                'texture': 'Floor_Modular_Texture.png',
                'rigid_body': False,
            })

        walls = []

        if lines[y - 1][x] == '.':
            walls.append([[x * 10.0, -2.4, (y * 10.0) - 5.0],
                          [0.0, 0.0, -1.570796327]])
        if lines[y + 1][x] == '.':
            walls.append([[x * 10.0, -2.4, (y * 10.0) + 5.0],
                          [0.0, 0.0, -1.570796327]])
        if lines[y][x - 1] == '.':
            walls.append([[(x * 10.0) - 5.0, -2.4, y * 10.0],
                          [0.0, 1.570796327, -1.570796327]])
        if lines[y][x + 1] == '.':
            walls.append([[(x * 10.0) + 5.0, -2.4, y * 10.0],
                          [0.0, 1.570796327, -1.570796327]])

        for wall in walls:
            zone['static_geometry'].append({
                'position': wall[0],
                'orientation': wall[1],
                'scale': [5.0, 5.0, 5.0],
                'mesh_type': 'Wall_Modular.fbx',
                'texture': 'Wall_Modular_Texture.png',
                'rigid_body': True,
                'rigid_body_type': 'bounding_box'
            })

        if cell_code == 'p':
            zone['player_start_position'] = [
                x * 10.0, 0.0, y * 10.0]

        if cell_code == 'e':
            enemy_x = x * 10.0
            enemy_y = y * 10.0
            min_x_till_wall = 0
            max_x_till_wall = 0
            min_y_till_wall = 0
            max_y_till_wall = 0

            for dx in range(1, len(lines[y])):
                if lines[y][x + dx] == '#':
                    max_x_till_wall += 1
                else:
                    break

            for dx in range(1, len(lines[y])):
                if lines[y][x - dx] == '#':
                    min_x_till_wall += 1
                else:
                    break

            for dy in range(1, len(lines)):
                if lines[y + dy][x] == '#':
                    max_y_till_wall += 1
                else:
                    break

            for dy in range(1, len(lines)):
                if lines[y - dy][x] == '#':
                    min_y_till_wall += 1
                else:
                    break

            zone['enemies'].append({
                'script': 'basic_enemy.lua',
                'mesh': 'Mushroom.fbx',
                'texture': 'Mushroom_Texture.png',
                'position': [enemy_x, -1.5, enemy_y],
                'orientation': [0.0, 0.0, 0.0],
                'bounds_min': [enemy_x - (min_x_till_wall * 10.0) - 5.0, 0.0, enemy_y - (min_y_till_wall * 10.0) - 5.0],
                'bounds_max': [enemy_x + (max_x_till_wall * 10.0) + 5.0, 0.0, enemy_y + (max_y_till_wall * 10.0) + 5.0],
                'scale': [0.01, 0.01, 0.01]
            })

        if cell_code == 'O':
            zone['portal'] = {
                'destination': 'town',
                'position': [x * 10.0, 0.0, y * 10.0],
                'scale': [0.5, 1.7, 0.5]
            }

print(yaml.dump(zone, default_flow_style=False))
