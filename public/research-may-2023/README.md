Looking at ways to upload images to tensorflowjs




Notes:


```
for(let i=1;i<=15;i++){
const im = new Image()
im.src = "eyes/"+i+".jpg";
im.onload = () => {
    const a = tf.browser.fromPixels(im, 4)
    a.print()
    console.log(a.shape)
}
}
```
