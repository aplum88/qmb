const fs = require('fs');

fs.open('hello.txt', 'a+', (err, fd) => {
  if (err) throw err;
	console.log('成功创建');
});
