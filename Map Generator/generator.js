const MAX_PIXEL_VALUE = 10000;
const MATRIX_LENGTH = Math.pow(2, 9) + 1;
const CANVAS_HEIGHT = MATRIX_LENGTH * 1.6;
const CANVAS_WIDTH = MATRIX_LENGTH * 1.6;
const MATRIX_DIMENSIONS = 
{
    pixelHeight: 2,
    pixelWidth: 2
};

const canvas = document.querySelector("canvas");
canvas.height = CANVAS_HEIGHT;
canvas.width = CANVAS_WIDTH;

const COLORS = 
[
    {
        terrainType: "mountains",
        hue: 30,
        saturation: 5,
        lightness: 25,
        variety: 200
    },
    {
        terrainType: "forest",
        hue: 120,
        saturation: 61,
        lightness: 20,
        variety: 20
    },    
    {
        terrainType: "plains",
        hue: 100,
        saturation: 50,
        lightness: 30,
        variety: 20
    },
    {
        terrainType: "beach",
        hue: 45,
        saturation: 35,
        lightness: 65,
        variety: 5
    },    
    {
        terrainType: "water",
        hue: 200,
        saturation: 90,
        lightness: 25,
        variety: 3
    }
];

/**
 * function that choses the pixel his color based on the value in the pixel
 */
function getColor(percentage) 
{
    const colorVariety = COLORS.length;
    const colorIndex = Math.floor(percentage * colorVariety);
    const {hue, saturation, lightness, variety} = COLORS[colorIndex] || COLORS[COLORS.length - 1];
    const finalVariaty = Math.floor(Math.random() * variety);
    const finalHue = (hue + finalVariaty) % 360;

    return `hsl(${finalHue}, ${saturation}%, ${lightness}%)`;
}

/**
 * Draws the colored pixels from the normalized matrix on the screen
 */
function draw(Matrix)
{
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, CANVAS_HEIGHT, CANVAS_WIDTH);
    ctx.beginPath();

    Matrix.forEach((pixelsRow, rowIndex) => 
    {
        const y = rowIndex * MATRIX_DIMENSIONS.pixelHeight;
        pixelsRow.forEach((pixel, pixelIndex) => 
        {
            const x = pixelIndex * MATRIX_DIMENSIONS.pixelWidth;
            ctx.fillStyle = getColor(pixel);
            ctx.fillRect(
            x,
            y,
            MATRIX_DIMENSIONS.pixelWidth,
            MATRIX_DIMENSIONS.pixelHeight
            );
        });
    });

    ctx.closePath();
}

/**
 * Takes a range of 2 numbers and returns a random number from that range
 */
function randomInRange(min, max) {
  return Math.floor(Math.random() * (max - min + 1) + min);
}

/**
 * Function that generates matrix with 0s and fills the four courners of the matrix with initial values
 */
function generateMatrix() 
{
    const matrix = new Array(MATRIX_LENGTH).fill(0).map(() => new Array(MATRIX_LENGTH).fill(null));

    matrix[0][0] = randomInRange(0, MAX_PIXEL_VALUE);  
    matrix[0][MATRIX_LENGTH - 1] = randomInRange(0, MAX_PIXEL_VALUE);
    matrix[MATRIX_LENGTH - 1][0] = randomInRange(0, MAX_PIXEL_VALUE);
    matrix[MATRIX_LENGTH - 1][MATRIX_LENGTH - 1] = randomInRange(0, MAX_PIXEL_VALUE);

    return matrix;
}
/**
 * For each square in the array, set the midpoint of that square to be the average of the four corner points plus a random value
 */
function DiamondStep(matrix, chunkSize, randomFactor) 
{
    for (var x = 0; x < matrix.length - 1; x += chunkSize) 
    {
        for (var y = 0; y < matrix.length - 1; y += chunkSize) 
        {
            const BOTTOM_RIGHT = matrix[y + chunkSize] ? matrix[y + chunkSize][x + chunkSize] : null;
            const BOTTOM_LEFT = matrix[y + chunkSize] ? matrix[y + chunkSize][x] : null;
            const TOP_LEFT = matrix[y][x];
            const TOP_RIGHT = matrix[y][x + chunkSize];
            // this calculates the final sum and count of the four corner points
            const { count, sum } = [BOTTOM_RIGHT, BOTTOM_LEFT, TOP_LEFT, TOP_RIGHT] 
            .reduce(
                (result, value) => 
                {
                    if (isFinite(value) && value != null) 
                    {
                        result.sum += value;
                        result.count += 1;
                    }
                    return result;
                },
                { sum: 0, count: 0 }
            );
            // Sets the midpoint to the average of the four + random value
            matrix[y + chunkSize / 2][x + chunkSize / 2] = sum / count + randomInRange(-randomFactor, randomFactor);
        }
    }
}
/**
 * For each diamond in the array, set the midpoint of that diamond to be the average of the four corner points plus a random value
 */
function Squarestep(matrix, chunkSize, randomFactor) 
{
    const half = chunkSize / 2;
    for (var y = 0; y < matrix.length; y += half) 
    {
        for (var x = (y + half) % chunkSize; x < matrix.length; x += chunkSize) 
        {
            const BOTTOM = matrix[y + half] ? matrix[y + half][x] : null;
            const LEFT = matrix[y][x - half];
            const TOP = matrix[y - half] ? matrix[y - half][x] : null;
            const RIGHT = matrix[y][x + half];
            // this calculates the final sum and count of the four corner points
            const { count, sum } = [BOTTOM, LEFT, TOP, RIGHT].reduce(
            (result, value) => 
            {
                if (isFinite(value) && value != null) 
                {   
                    result.sum += value;
                    result.count += 1;
                }
                return result;
            },
            { sum: 0, count: 0 }
            );
            // Sets the midpoint to the average of the four + random value
            matrix[y][x] = sum / count + randomInRange(-randomFactor, randomFactor);
        }
    }
    return matrix;
}
/**
 * Calculates all the array values 
 */
function diamondSquare(matrix) 
{
    var chunkSize = MATRIX_LENGTH - 1;
    var randomFactor = MAX_PIXEL_VALUE;

    while (chunkSize > 1) 
    {
        DiamondStep(matrix, chunkSize, randomFactor);
        Squarestep(matrix, chunkSize, randomFactor);
        chunkSize /= 2;
        randomFactor /= 2;
    }

    return matrix;
}
/**
 * Normalizes the matrix to values between 0 and 1
 */
function normalizeMatrix(matrix) 
{
    const maxValue = matrix.reduce((max, row) => {return row.reduce((max, value) => Math.max(value, max));}, -Infinity);

    return matrix.map((row) =>{return row.map((val) => val / maxValue);});
}

/**
 * Main function of the program
 */
function start() 
{
    const matrix = diamondSquare(generateMatrix());
    const normalizedMatrix = normalizeMatrix(matrix);
    draw(normalizedMatrix);
}

start();

document.querySelector("button").addEventListener("click", start);