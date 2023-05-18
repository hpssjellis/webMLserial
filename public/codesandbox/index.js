const { exec } = require("child_process");

exec(
  "tensorflowjs_converter --input_format=tfjs_layers_model --output_format=keras_saved_model ./model.json ./",
  (error, stdout, stderr) => {
    if (error) {
      console.error(`error: ${error.message}`);
      return;
    }
    if (stderr) {
      console.error(`stderr: ${stderr}`);
      return;
    }
    console.log(`stdout: ${stdout}`);
  }
);

exec(
  "tflite_convert --keras_model_file ./ --output_file ./model.tflite",
  (error, stdout, stderr) => {
    if (error) {
      console.error(`error: ${error.message}`);
      return;
    }
    if (stderr) {
      console.error(`stderr: ${stderr}`);
      return;
    }
    console.log(`stdout: ${stdout}`);
  }
);
