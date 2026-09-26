<h1 align="center">Game Object Sequences</h1>

This document describes the runtime sequence of objects in game. Each object which have own AK task for receive and handle signals from the screen task (scr_play.c) such as button callbacks, the periodic game tick timer...

## I. Object Summary

| Object | Task ID | Handler | Main responsibility |
|---|---|---|---|
| T-Rex | `TINY_REX_OBJECT_ID` | `tiny_rex_object_handle()` | Controls the player position, movement, and image state. |
| Obstacles | `OBSTACLE_OBJECT_ID` | `obstacle_object_handle()` | Controls obstacle spawning, movement, and collision with T-Rex. |
| Horizon | `HORIZON_OBJECT_ID` | `horizon_object_handle()` | Controls the background and horizon elements during gameplay. |

## II. T-Rex Object Sequence

The Game Screen controls the gameplay flow and sends events to the T-Rex object.

**Start-Play.** `TINY_REX_PLAY_EVENT` initializes the T-Rex with:

- `state = RUNNING`
- Initial position
- Initial image
- Initial speed

**Per-move tick.** Each `TINY_REX_MOVE_EVENT` updates the T-Rex according to its current state:

- `RUNNING`: Updates the running animation frame.
- `JUMPING`: Moves the T-Rex up. The height above the ground is read from the first half of the jump height table of the current level.
- `FALLING`: Moves the T-Rex down by reading the second half of the same table, until the table is finished and the T-Rex lands.
- `DUCKING`: Updates the duck animation frame.

**Button control.** The Game Screen sends control events directly to the T-Rex:

- `BUTTON_UP_PRESS` -> `TINY_REX_JUMP`
  - If `state == RUNNING`, changes the state to `JUMPING` and selects the jump height table of the current level.
- `BUTTON_DOWN_PRESS` -> `TINY_REX_FALL`
  - If `state == JUMPING` or `state == FALLING`, changes the state to `FALLING` and falls 2 times faster (2 table entries for each update). If the T-Rex is still going up it continues down from the same height, so it never goes higher after the button is pressed.
- `BUTTON_MODE_PRESS` -> `TINY_REX_DUCK`
  - If `state == RUNNING`, changes the state to `DUCKING`.

**Jump height tables.** There is one table for each level (`TINY_REX_JUMP_HEIGHT_L1` to `TINY_REX_JUMP_HEIGHT_L4` in `tiny_rex_object.cpp`). Every table goes up to the same top (36 pixels) and comes back down, one entry for each update (50ms). The higher the level, the faster the obstacles come, so the table is shorter (18, 13, 12 and 11 entries). The table is chosen when the jump starts and used for the whole jump.

**Difficulty increase.** The T-Rex starts at `L1`. `TINY_REX_INC_SPEED_EVENT` increases the level (up to `L4`), so the next jump uses a shorter jump height table. The Score handler sends this event every 200 points.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%
sequenceDiagram
    autonumber

    participant Scr as Game Screen
    participant Rex as T-Rex
    participant Score as Score

    %% ==================================================
    %% T-Rex setup
    %% ==================================================

    Scr->>Rex: TINY_REX_PLAY_EVENT
    activate Rex
    Note right of Rex: State = RUNNING<br/>Set initial position<br/>Set initial image<br/>Set speed
    deactivate Rex

    %% ==================================================
    %% T-Rex Running
    %% ==================================================
    Note over Scr,Rex: T-Rex Running

    Scr->>Rex: TINY_REX_MOVE_EVENT
    activate Rex

    alt State = RUNNING
        Note right of Rex: Update running animation frame
    else State = JUMPING
        Note right of Rex: Y = ground - table[index] <br/> index + 1
        alt First half of the table is done
            Note right of Rex: State = FALLING
        end
    else State = FALLING
        Note right of Rex: Y = ground - table[index] <br/> index + 1 or 2 after fast-fall
        alt Table is finished
            Note right of Rex: State = RUNNING
        end
    else State = DUCKING
        Note right of Rex: Update duck animation frame
    end
    deactivate Rex

    %% ==================================================
    %% Button Control
    %% ==================================================
    alt BUTTON_UP_PRESS
        Scr->>Rex: TINY_REX_JUMP
        activate Rex
        alt State == RUNNING
            Note right of Rex: State = JUMPING <br/> Select jump height table of the level
        end
        deactivate Rex

    else BUTTON_DOWN_PRESS
        Scr->>Rex: TINY_REX_FALL
        activate Rex
        alt State == JUMPING <br/> State == FALLING
            Note right of Rex: State = FALLING <br/> Fall 2 times faster
        end
        deactivate Rex

    else BUTTON_MODE_PRESS
        Scr->>Rex: TINY_REX_DUCK
        activate Rex
        alt State == RUNNING
            Note right of Rex: State = DUCKING
        end
        deactivate Rex
    end

    %% ==================================================
    %% Speed Increase
    %% ==================================================
    Note over Scr,Rex: Difficulty Increase

    Score->>Rex: TINY_REX_INC_SPEED_EVENT
    activate Rex
    Note right of Rex: Increase level <br/> next jump uses a shorter table
    deactivate Rex
```

<p align="center"><strong><em>Figure 1:</em></strong> Tiny-Rex sequence logic</p>

# III. Obstacles Object Sequence

The Game Screen controls the gameplay flow and sends events to the Obstacle object. The Obstacle object manages obstacle spawning, movement, spacing, and collision detection with the T-Rex.

**Start-Play.** `OBSTACLE_PLAY_EVENT` initializes the obstacle system with:

* Initialize the obstacle list.
* Create the first obstacle.
* `state = RUNNING`
* Set the initial position.
* Set the initial speed.
* Set the minimum gap between obstacles.

**Per-move tick.** Each `OBSTACLE_MOVE_EVENT` updates all active obstacles:

* Moves obstacles from right to left:
  `X = X - speed`
* Updates the obstacle animation.
* Removes or disables obstacles that move outside the screen.
* Creates a new obstacle when the distance from the previous obstacle reaches the minimum gap.
* Selects the obstacle type when creating a new obstacle.

**Collision detection.** `COLLISION_CHECK_EVENT` checks whether any active obstacle collides with the T-Rex.

* If a collision is detected, the Obstacle object sends `GAME_OVER_EVENT` to the Game Screen.
* The Game Screen changes to the `OVER` state and stops the game update.

**Difficulty increase.** `OBSTACLE_INC_SPEED_EVENT` increases the obstacle speed and reduces the minimum gap distance, making the game progressively more difficult.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%

sequenceDiagram
autonumber

participant Scr as Game Screen
participant Obs as Obstacle

%% ==================================================
%% Obstacle Play / Initial Setup
%% ==================================================

Scr->>Obs: OBSTACLE_PLAY_EVENT
activate Obs

Note right of Obs: Initialize obstacle list<br/>Create first obstacle<br/>State = RUNNING<br/>Set initial position<br/>Set initial speed<br/>Set initial gap distance

deactivate Obs

%% ==================================================
%% Obstacle Update
%% ==================================================

Note over Scr,Obs: Obstacle Running

loop UPDATE SCREEN

    Scr->>Obs: OBSTACLE_MOVE_EVENT
    activate Obs

    Note right of Obs: Move all active obstacles<br/>X = X - speed<br/>Update animation

    alt Obstacle is outside screen
        Note right of Obs: Remove / Disable obstacle
    end

    alt Distance from last obstacle >= minimum gap
        Note right of Obs: Select obstacle type<br/>Create new obstacle
    end

    deactivate Obs

    %% --------------------------------------------------
    %% Collision
    %% --------------------------------------------------

    Scr->>Obs: COLLISION_CHECK_EVENT
    activate Obs

    alt Collision detected
        Obs->>Scr: GAME_OVER_EVENT
        Note left of Scr: State = OVER<br/>Stop game update
    end

    deactivate Obs

end

%% ==================================================
%% Speed Increase
%% ==================================================

Score->>Obs: OBSTACLE_INC_SPEED_EVENT
activate Obs

Note left of Obs: Increase obstacle speed<br/>Decrease minimum gap distance

deactivate Obs
```

# IV. Horizon Object Sequence

The Game Screen controls the gameplay flow and sends events to the Horizon object. The Horizon object manages the movement and recycling of background horizon elements to create a continuous scrolling effect.

**Start-Play.** `HORIZON_PLAY_EVENT` initializes the Horizon object with:

* `state = RUNNING`
* Initial position
* Initial image
* Initial speed

**Per-move tick.** Each `HORIZON_MOVE_EVENT` updates the Horizon objects:

* Moves the horizon objects from right to left:
  `X = X - speed`
* When a horizon object moves outside the screen, its position is reset to the right side of the screen so it can re-enter the gameplay area.

**Continuous scrolling.** The Horizon object continuously moves and recycles its elements while the game is running, creating the scrolling background effect.

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%

sequenceDiagram
autonumber

participant Scr as Game Screen
participant Hor as Horizon

%% ==================================================
%% Horizon Play / Initial Setup
%% ==================================================

Scr->>Hor: HORIZON_PLAY_EVENT
activate Hor

Note right of Hor: State = RUNNING<br/>Set initial position<br/>Set initial image<br/>Set speed

deactivate Hor

%% ==================================================
%% Horizon Update
%% ==================================================

Note over Scr,Hor: Horizon Running

loop UPDATE SCREEN

    Scr->>Hor: HORIZON_MOVE_EVENT
    activate Hor

    Note right of Hor: Move horizon objects<br/>X = X - speed

    alt Horizon object is outside screen
        Note right of Hor: Reset object position<br/>to screen right
    end

    deactivate Hor

end
```

# V. Score Sequence

The Game Screen controls the gameplay flow and sends events to the Score handler. The Score handler manages the score, update highest score from memory and difficult level.

**Start-Play.** `SCORE_SETUP` initializes the Score handler to default:

* Clear Current score

**Per-move tick.** Each `SCORE_UPDATE` updates the score and it will compare with score threshold for increase difficult level. The threshold starts at 200 points and goes up by 200 every time it is reached (200, 400, 600 ...). When difficult level increase, It will send event to object for increase velocity.
* Score -> T-Rex: `TINY_REX_INC_SPEED_EVENT`
* Score -> Obstacles: `OBSTACLE_INC_SPEED_EVENT`

**Game Over** `GAME_OVER_EVENT` It will save current socre. If current socre exceed highest score then it save score to memroy. And display GAME OVER icon

```mermaid
%%{init: {'theme':'base','themeVariables':{'fontSize':'18px','primaryColor':'#1565c0','primaryTextColor':'#ffffff','primaryBorderColor':'#0d47a1','lineColor':'#90a4ae','signalColor':'#ffc107','signalTextColor':'#ffc107','actorBkg':'#1565c0','actorBorder':'#0d47a1','actorTextColor':'#ffffff','actorLineColor':'#90caf9','noteBkgColor':'#fff59d','noteTextColor':'#000000','noteBorderColor':'#f57f17','activationBkgColor':'#66bb6a','activationBorderColor':'#2e7d32','sequenceNumberColor':'#ffffff','loopTextColor':'#ffc107','labelBoxBkgColor':'#37474f','labelBoxBorderColor':'#90a4ae','labelTextColor':'#ffffff'},'sequence':{'actorMargin':120,'messageFontSize':17,'noteFontSize':15,'actorFontSize':17,'boxMargin':15,'boxTextMargin':8,'noteMargin':12,'useMaxWidth':false}}}%%

sequenceDiagram
autonumber

participant Scr as Game Screen
participant Score as Score

%% ==================================================
%% Score Play / Initial Setup
%% ==================================================

Scr->>Score: SCORE_SETUP
activate Score
Note right of Score: Clear current score
deactivate Score

%% ==================================================
%% Score Update
%% ==================================================

Note over Scr,Score: Game Playing

loop UPDATE SCORE

    Scr->>Score: SCORE_UPDATE_EVENT
    activate Score
    Note over Score: Increase score
    alt current score > threshlod
        Note over Score: Increase threshold
        Score->>T-Rex: TINY_REX_INC_SPEED_EVENT
        Score->>Obstacle: OBSTACLE_INC_SPEED_EVENT
    end

    deactivate Score

end
```

## V. Code References

| Object   | Source file                                            | Header file                                          |
| -------- | ------------------------------------------------------ | ---------------------------------------------------- |
| T-Rex    | `application/sources/app/tiny_rex/tiny_rex_object.cpp` | `application/sources/app/tiny_rex/tiny_rex_object.h` |
| Obstacle | `application/sources/app/obstacle/obstacle_object.cpp` | `application/sources/app/obstacle/obstacle_object.h` |
| Horizon  | `application/sources/app/horizon/horizon_object.cpp`   | `application/sources/app/horizon/horizon_object.h`   |
| Scire    | `application/sources/app/horizon/score.cpp`            | `application/sources/app/horizon/score.h`             |