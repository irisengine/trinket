# Trinket
Trinket is a bare bones Action RPG built with [iris](https://github.com/irisengine/iris). It's less of a game and more of a proving ground for the engine, the game features:
* Level loading
* Enemies (lua AI)
* XP & levelling
* Quests

# Design
The game design is very simple, using classic object inheritance rather than an ECS. There is a base `GameObject` and they communicate via a `MessageBroker` (pubsub system). Game data is described in a series of YAML files.

Assets from [Quaternius](https://quaternius.com/).

![Screenshot](media/screen.png)

[Watch the video](https://www.youtube.com/watch?v=iBDA1-bGjAY) on how it was made!
