const { exec } = require('child_process');

exec('tensorflowjs_converter --input_format=tfjs_layers_model --output_format=keras_saved_model ./$myFile ./', (err, stdout, stderr) => {
  if (err) {
    console.error(err);
    return;
  }
  console.log(stdout);
});

exec('tflite_convert --keras_model_file ./ --output_file ./model.tflite', (err, stdout, stderr) => {
  if (err) {
    console.error(err);
    return;
  }
  console.log(stdout);
});

exec('xxd -i model.tflite model.h', (err, stdout, stderr) => {
  if (err) {
    console.error(err);
    return;
  }
  console.log(stdout);
});
