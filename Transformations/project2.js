function GetTransform(positionX, positionY, rotation, scale) {

  // Matrix for scaling
  let matrixScale = 
      [scale, 0,     0,
      0,      scale, 0,
      0,      0,     1];

  // Matrix for rotation    
  let matrixRotation = 
      [Math.cos(rotation * Math.PI / 180),    Math.sin(rotation * Math.PI / 180), 0,
      -Math.sin(rotation * Math.PI / 180),    Math.cos(rotation * Math.PI / 180), 0,
       0,                                     0,                                  1];

  // Matrix for translation     
  let matrixTranslation = 
      [1,           0,          0,
       0,           1,          0,
       positionX,   positionY,  1];

  // Here is the sweet part, we can use the ApplyTransform function to combine the matrices.
  let resultMatrix = ApplyTransform(matrixScale, matrixRotation);
  resultMatrix = ApplyTransform(resultMatrix, matrixTranslation);

  // Return the result
  return resultMatrix;
}

function ApplyTransform(trans1, trans2) {

  // Initialize a new 3x3 matrix and fill it with zeros.
  let matrix = Array(9).fill(0);

  // Triple nested loop performs matrix multiplication between trans1 and trans2.
    for (var i = 0; i < 3; i++) {
      for (var j = 0; j < 3; j++) {
        for (var k = 0; k < 3; k++) {
          matrix[i * 3 + j] += trans1[i * 3 + k] * trans2[k * 3 + j];
        }
      }
    }

    /*  Alternative straightforward solution:
        let matrix = [
  trans1[0]*trans2[0] + trans1[1]*trans2[3] + trans1[2]*trans2[6],
  trans1[0]*trans2[1] + trans1[1]*trans2[4] + trans1[2]*trans2[7],
  trans1[0]*trans2[2] + trans1[1]*trans2[5] + trans1[2]*trans2[8],
  trans1[3]*trans2[0] + trans1[4]*trans2[3] + trans1[5]*trans2[6],
  trans1[3]*trans2[1] + trans1[4]*trans2[4] + trans1[5]*trans2[7],
  trans1[3]*trans2[2] + trans1[4]*trans2[5] + trans1[5]*trans2[8],
  trans1[6]*trans2[0] + trans1[7]*trans2[3] + trans1[8]*trans2[6],
  trans1[6]*trans2[1] + trans1[7]*trans2[4] + trans1[8]*trans2[7],
  trans1[6]*trans2[2] + trans1[7]*trans2[5] + trans1[8]*trans2[8]];
      */

  // Return the result.
  return matrix;
}