// regex to split on dots while preserving quoted segments
// so you can use a path like: 
// set(obj, 'foo."bar.baz".qux', value)
// to set obj.foo["bar.baz"].qux = value
const regex = /"[^"]*"|[^.]+/g;

// immutable set function for nested objects/arrays
export const set = (obj, path, value) => {
    if (!path || path == "") return value;

    const steps = [...path.matchAll(regex)].map(match => match[0].replace(/"/g, ''));

    const recursiveSet = (obj, steps) => {
        if (steps.length === 0) return value;

        const [step, ...remainingSteps] = steps;

        if (Array.isArray(obj)) {
            const index = parseInt(step);
            const result = [...obj];
            result[index] = recursiveSet(obj[index], remainingSteps);
            return result;
        } else if (typeof (obj) === "object") {
            const result = { ...obj };
            result[step] = recursiveSet(obj[step], remainingSteps);
            return result;
        } else {
            console.error('unknown obj', obj, typeof (obj))
        }
    }

    return recursiveSet(obj, steps);
}
