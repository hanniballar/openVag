#pragma once

#include "../IRModel.h"
#include "../commands/CommandCenter.h"

bool isValidPasteCliboard();
bool pasteCliboardText(std::shared_ptr<IRModel> irModel, CommandCenter& commandCenter);
void displayPasteError();