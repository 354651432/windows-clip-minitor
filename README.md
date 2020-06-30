# windows-clip-minitor

listen clipboard change, write content to file ,or send http post
`config.ini`
```ini
[main]
filename=clip.txt  # file name to write content, if not set no file will be written
host=1.1.1.1  # http host
port=9000 # http port `optional` default 80
path=updateclip # http path `optional` default /
```
