var addon = require('bindings')('spellchecker');

var obj = new addon.Spellchecker(10);

console.log( obj.isMisspelled("helloo") );
console.log( obj.isMisspelled("hello") );
console.log( obj.checkSpelling("helloo").join(', '));