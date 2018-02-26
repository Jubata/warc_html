# warc_html
It is unsuccessful attempt to parse html tags form WARC files. The parsing rate is approximately 30MB/s. Code is ugly and slow. There are more convinint tools to perform this task better (go, node.js, python or maybe rust). 

### How to build 
```
git clone --recurse-submodules git@github.com:Jubata/warc_html.git
cd warc_html
meson out
ninja -C out
```
