export const set = (obj, path, value) => {
    if (!path || path == "") return value;

    const steps = path.split(".");
    const step = steps[0];
    const remainingSteps = steps.slice(1).join(".");

    if (Array.isArray(obj)) {
        const index = parseInt(step);
        const result = [...obj];
        result[index] = set(obj[index], remainingSteps, value);
        return result;
    } else if (typeof (obj) === "object") {
        const result = { ...obj };
        result[step] = set(obj[step], remainingSteps, value);
        return result;
    } else {
        console.error('unknown obj', obj, typeof (obj))
    }
}