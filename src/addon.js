var addon = require('bindings')('spellchecker');

var obj = new addon.Spellchecker(10);

console.log( obj.isMisspelled("helloo") );
console.log( obj.isMisspelled("hello") );
console.log( obj.getCorrectionsForMisspelling("helloo").join(', '));
var misRanges = obj.checkSpelling("helloo wrold");
for (var i = 0; i < misRanges.length; i++) {
    console.log(`start: ${misRanges[i].start}, end: ${misRanges[i].end}`);
}
console.log( obj.getAvailableDictionaries().join(', ') );
