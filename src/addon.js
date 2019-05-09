var addon = require('bindings')('spellchecker');
var path = require('path');

var obj = new addon.Spellchecker();

// console.log( obj.add("cliché") );
// console.log( obj.getCorrectionsForMisspelling("reborni") );
// console.log( obj.getAvailableDictionaries() );
var dictionaryDirectory = path.join(__dirname, '../', 'spec', 'dictionaries')

let string = "'cat' (caat. <dog> :dooog)"
let ret = obj.setDictionary('fr', dictionaryDirectory);
console.log(ret);
let ranges = obj.checkSpelling(string);
for (var i = 0; i < ranges.length; i++) {
    console.log(ranges[i].start, ranges[i].end);
}
