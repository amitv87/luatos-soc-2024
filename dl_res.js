const fs = require('fs');
const crypto = require('crypto');
const child_process = require('child_process');

const meta = require(process.argv[2]);
const f_plat = process.argv[3];
// console.log('meta', meta);

const path = 'http://xmake.vue2.cn/xmake/libs';

if(f_plat) meta.libs = {[f_plat]: meta.libs[f_plat]};
else if(meta.prebuild) meta.libs.prebuild = meta.prebuild;

const plats = Object.keys(meta.libs);

for(var i = 0; i < plats.length; i++){
  var plat = plats[i];
  var file  = `${plat}.7z`;
  var url = `${path}/${plat}/${meta.libs[plat].sha1}.7z`;

  if(fs.existsSync(file)){
    if(crypto.createHash('sha1').update(fs.readFileSync(file)).digest("hex") == meta.libs[plat].sha1) continue;
    fs.unlink(file)
  }

  console.log('fetching', url);
  child_process.execFileSync('curl', ['-Lo', file, url]);
}
