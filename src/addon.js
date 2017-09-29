var addon = require('bindings')('spellchecker');

var obj = new addon.Spellchecker();

// console.log( obj.add("cliché") );
console.log( 'Is hello misspelled? ' + obj.isMisspelled("hello") );
console.log( 'Is helloo misspelled? ' + obj.isMisspelled("helloo") );
console.log( 'Corrections for helloo: ' + obj.getCorrectionsForMisspelling("helloo").join(', ') );
console.log( 'Dictionaries: ' + obj.getAvailableDictionaries().join(', ') );
console.log( 'Dictionaries: ' + obj.getAvailableDictionaries('.').join(', ') );

