export const viewParams = {
    clearColor: [0.1, 0.1, 0.1, 1.0],
    fieldOfView: (80 * Math.PI) / 180,
    gridZ: -0.45,
    transform: [
        {
            //move 2 units back
            type:'translate',
            amount: [0,0,-2]
        },
        {
            // look slightly from below
            type:'rotate',
            amount: (_t)=>-0.1,
            vector:[1,0,0]
        },
        {
            //rotate the scene
            type:'rotate',
            amount: (t)=>t/8,
            vector: [0,1,0]
        }
    ]
}