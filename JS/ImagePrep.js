// Setup
"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });

const fs = __importDefault(require("fs")); //Allows access to the file system
const jimp = require("jimp"); //Used to save BMPs with the correct format, unlike fs
const fs_1_e = require("fs-extra"); //Includes extra functions for fs

//Dimensions of the screen
const width = 320;
const height = 480;
let i = 0; //Loop counter

//Functions

const findImages = () => {
    const files = fs.default.readdirSync(`./Input`); //Reads the Input folder in same folder as the script
    const images = files.map((file) => fs.default.readFileSync(`./Input/${file}`)); //Return the files in the folder
    console.log(`Found ${images.length} images.`);
    if (images.length == 0) { //If there are no images in the Input folder, exit the script
        process.exit();
    }
    const outFiles = fs.default.readdirSync('./Output'); //Reads the Output folder in same folder as the script
    const outImg = outFiles.map((file) => fs.default.readFileSync(`./Output/${file}`)); //Return the files in the folder
    outImg.length !== 0 ? console.log(`Warning - There are ${outImg.length} images in the output folder. These will be overwritten`) : console.log();
    fs_1_e.emptyDirSync('./Output'); //Clears the output folder
    return images; //Return an array of the images found
};

const editImage = async (img) => {
    try {
        const image = await jimp.read(img);

        const dimensions = {
            width: image.getHeight(),
            height: image.getWidth()
        };
        //Define the dimensions to resize the image to
        //console.log(`Width and Height: ${dimensions.width} x ${dimensions.height}`);
        const newWidth = dimensions.width >= dimensions.height ? 320 : Math.min(dimensions.width, dimensions.height) / Math.max(dimensions.width, dimensions.height) * 480;
        const newHeight = dimensions.height > dimensions.width ? 480 : Math.min(dimensions.width, dimensions.height) / Math.max(dimensions.width, dimensions.height) * 320;
        //console.log(`Width and Height: ${newWidth} x ${newHeight}`);
        const X = newHeight === 480 ? (width - newWidth) / 2 : 0;
        const Y = newWidth === 320 ? (height - newHeight) / 2 : 0;
        //Set background colour to black
        let background = new jimp(width, height, '0xffffffff', (err, background) => {
            if (err) throw err;
        });
        //Rotate, then resize the image
        image.rotate(90);
        image.resize(newWidth, newHeight);
        background.composite(image, X, Y);//Places the resized image ontop of the background at the center of the image
        const name = i++;
        console.log(`Formatted image ${name}`);
        return background.write(`./Output/${name}.bmp`);
    } catch (err) {
        console.error(err.stack);
    }
};

const editAll = async () => {
    console.log("-------- Image Preparation --------");
    console.log("This script will format your images for use on your display");
    let imgs = findImages();
    console.log("Formatting Images. Please wait...");
    for (const image of imgs) {
        await editImage(image);
    }
    console.log("Operations complete! Please check the Output folder for new images.");
};

editAll(); //Program begin!

/*Code and commentary by SA
With thanks to DTB and OATJ*/
