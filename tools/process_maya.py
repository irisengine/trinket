import math
import pymel.core as pm
import yaml
import os
import tempfile

zone = {
    'player_start_position': [0.0, 0.0, 0.0],
    'name': 'town3',
    'portal': {
        'destination': 'town',
        'position': [-23.0, 1.0, 0.0],
        'scale': [1.0, 1.0, 1.0]
    },
    'static_geometry': [{
        'position': [0.0, -1000.0, 0.0],
        'orientation': [0.0, 0.0, 0.0],
        'scale': [1000.0, 1000.0, 1000.0],
        'mesh_type': 'cube',
        'texture': ['grass.png'],
        'rigid_body': True,
        'rigid_body_type': 'bounding_box'
    }],
    'enemies': []
}

game_meshes = [
    'Blacksmith',
    'Inn',
    'TallWall',
    'Bell_Tower',
    'SawMill',
    'Stable',
    'Mill',
    'PointyTower',
    'House_1',
    'House_2',
    'PineTree_1',
    'PineTree_2',
    'PineTree_3',
    'PineTree_5',
]

textures = {
    'PineTree_1': ['PineTree_Bark.png', 'PineTree_Leaves.png'],
    'PineTree_2': ['PineTree_Bark.png', 'PineTree_Leaves.png'],
    'PineTree_3': ['PineTree_Bark.png', 'PineTree_Leaves.png'],
    'PineTree_5': ['PineTree_Bark.png', 'PineTree_Leaves.png'],
}

normals = {
    'PineTree_1': 'PineTree_Bark_Normal.png',
    'PineTree_2': 'PineTree_Bark_Normal.png',
    'PineTree_3': 'PineTree_Bark_Normal.png',
    'PineTree_5': 'PineTree_Bark_Normal.png',
}

for mesh in pm.ls(type='mesh'):
    transform = mesh.getTransform()
    translation = transform.getTranslation('world')
    rotation = transform.getRotation()
    scale = transform.getScale()

    if 'spawn' in mesh.shortName():
        zone['player_start_position'] = [
            translation.x, translation.y, translation.z]
    else:
        for gm in game_meshes:
            if gm in mesh.shortName():
                convert = math.pi / 180.0

                zone['static_geometry'].append({
                    'position': [translation.x, translation.y, translation.z],
                    'orientation': [0, rotation.y*convert, -math.pi / 2],
                    'scale': [scale[0], scale[1], scale[2]],
                    'mesh_type': gm + '.fbx',
                    'rigid_body': True,
                    'rigid_body_type': 'mesh'
                })

                if gm in textures:
                    zone['static_geometry'][-1]['texture'] = textures[gm]
                if gm in normals:
                    zone['static_geometry'][-1]['normal'] = normals[gm]

yaml_str = yaml.dump(zone, default_flow_style=False)
print(yaml_str)
