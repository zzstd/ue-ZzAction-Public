// Copyright 2024-2026 zz studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/*
 * 
 * Pact is a module used to handle precise alignment of multiple character.
 * It is usually used for execute, opening doors, and other interactive animations, similar to Contextual Animation.
 * 
 * It is based on the LinkNode + MotionWarping solution.
 * 
 * Use:
 *  1. Add UZzActionPactUserData to ActionBlueprint to use it normally 
 *      or click the QuickCreatePact button in the toolbar to quickly create it
 *      
 *  2. In LinkNode, you need to use UZzActionNE_PactRunLinkAction to run link action 
 *      or use UZzActionPactLibrary::PushPactData in your code
 *      
 *  3. Set RelativeTransform in LinkNode to position the character correctly.
 *  
 *  4. Set Anchor. Keep the stationary character as an anchor point，This depends on the Animation design.
 *      Usually:
 *          When a player executes an enemy, the enemy becomes the anchor point, and the player Warping onto the enemy.
 *      
 *  5. Click the Update Links button, and you can open the debug drawing button for non anchor characters
 */
class FZzActionPactModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
