var addon = require('bindings')('spellchecker');

var obj = new addon.Spellchecker();

console.log( obj.add("cliché") );
console.log( obj.checkSpelling() );

