RoamingState = {}

function RoamingState:new()
    next_animation = 'Walk'
    change_animation = true

    local o = {}
    setmetatable(o, RoamingState)
    RoamingState.__index = RoamingState
    return o
end

function RoamingState:update(enemy_position, player_position)
    if (find_target == nil or find_target == true) then
        target = Vector3(math.random(bounds_min_x, bounds_max_x), 0.0, math.random(bounds_min_z, bounds_max_z))
        local dx = target:x() - enemy_position:x()
        local dz = target:z() - enemy_position:z()
        local theta = math.atan(dx, dz)

        orientation = Quaternion(Vector3(0.0, 1.0, 0.0), theta)

        find_target = false
    end

    local walk_dir = target - enemy_position
    walk_dir:normalise()
    walk_direction = walk_dir

    if ((target - enemy_position):magnitude() < 1.0) then
        find_target = true
    end

    local enemy_to_target = target - enemy_position
    local enemy_to_player = player_position - enemy_position
    local theta = math.acos(enemy_to_target:dot(enemy_to_player) /
                                (enemy_to_target:magnitude() * enemy_to_player:magnitude()))

    if (math.abs(theta) < math.pi / 4.0) then
        state = HuntingState:new{}
    end
end

HuntingState = {}

function HuntingState:new()
    find_target = true

    local o = {}
    setmetatable(o, HuntingState)
    HuntingState.__index = HuntingState
    return o
end

function HuntingState:update(enemy_position, player_position)
    local walk_dir = player_position - enemy_position
    local distance = walk_dir:magnitude()
    walk_dir:normalise()
    walk_direction = walk_dir

    local dx = player_position:x() - enemy_position:x()
    local dz = player_position:z() - enemy_position:z()
    local theta = math.atan(dx, dz)

    orientation = Quaternion(Vector3(0.0, 1.0, 0.0), theta)

    local px = player_position:x()
    local pz = player_position:z()

    if (px < bounds_min_x or px > bounds_max_x or pz < bounds_min_z or pz > bounds_max_z) then
        state = RoamingState:new{}
    elseif (distance < attack_distance) then
        state = AttackingState:new{}
    end
end

AttackingState = {}

function AttackingState:new()
    next_animation = 'Bite_Front'
    change_animation = true
    next_attack = elapsed_us + attack_rate

    local o = {}
    setmetatable(o, AttackingState)
    AttackingState.__index = AttackingState
    return o
end

function AttackingState:update(enemy_position, player_position)
    local dx = player_position:x() - enemy_position:x()
    local dz = player_position:z() - enemy_position:z()
    local theta = math.atan(dx, dz)

    orientation = Quaternion(Vector3(0.0, 1.0, 0.0), theta)
    walk_direction = Vector3(0.0, 0.0, 0.0)

    local distance = (player_position - enemy_position):magnitude()
    if (distance >= attack_distance) then
        state = HuntingState:new{}
    elseif (elapsed_us > next_attack) then
        has_attacked = true
        next_attack = elapsed_us + attack_rate
    end
end

function init(bounds_min, bounds_max)
    walk_direction = Vector3(-1.0, 0.0, 0.0)
    orientation = Quaternion(Vector3(0.0, 1.0, 0.0), 0.0)
    state = RoamingState.new()

    bounds_min_x = bounds_min:x()
    bounds_max_x = bounds_max:x()
    bounds_min_z = bounds_min:z()
    bounds_max_z = bounds_max:z()

    change_animation = false
    next_animation = ''

    attack_distance = 2.5
    has_attacked = false

    elapsed_us = 0
    attack_rate = 500000
    next_attack = 0

    math.randomseed(os.time())
end

function update(enemy_position, player_position, elapsed, health)
    elapsed_us = elapsed

    if (health <= 0.0) then
        next_animation = 'Death_Back'
        change_animation = true
        walk_direction = Vector3(0.0, 0.0, 0.0)
    else
        state:update(enemy_position, player_position)
    end
end

function get_walk_direction()
    return walk_direction
end

function get_orientation()
    return orientation
end

function get_animation_change()
    local change = change_animation
    change_animation = false

    return change, next_animation
end

function attack_player()
    local attack = has_attacked
    has_attacked = false

    return attack
end
