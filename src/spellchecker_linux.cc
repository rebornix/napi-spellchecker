/*
 * The file was derived from src/spellchecker_linux.cc in https://github.com/atom/node-spellchecker
 */

#include "spellchecker.h"
#include "spellchecker_hunspell.h"

namespace spellchecker {

SpellcheckerImplementation* SpellcheckerFactory::CreateSpellchecker() {
  return new HunspellSpellchecker();
}

}  // namespace spellchecker
