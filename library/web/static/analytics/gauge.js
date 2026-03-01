//based on https://dev.to/madsstoumann/how-to-create-gauges-in-css-3581

import { html } from '../common/preact-standalone.js'

export const AnalogGauge = ({
  value = 0,
  max = 100,
  min = 0,
  suffix = '',
  label = '',
  minLabel = '',
  maxLabel = '',
  values = '',
  className = '',
  
  maxDegree = 250,
  startAngle = -35,
}) => {
  const calculateDegree = (value) => {
    const normalizedValue = Math.max(min, Math.min(max, value));
    const valuePercentage = (normalizedValue - min) / (max - min);
    return startAngle + (valuePercentage * maxDegree);
  }

  const generateValueMarks = () => {
    if (!values) return [];

    let valueArray = [];
    let count = 0;

    if (/^\s*\d+\s*$/.test(values)) {
      count = parseInt(values.trim());
      if (isNaN(count) || count <= 0) return [];
      valueArray = Array.from({ length: count }, (_, i) =>
        Math.round(min + (i * (max - min) / (count - 1 || 1)))
      );
    } else {
      valueArray = values.split(',').map(v => v.trim());
      count = valueArray.length;
      if (count <= 0) return [];
    }

    return valueArray.map((val) => {
      return html`<li class="value-mark" style=${{ '--_d': `${calculateDegree(val)}deg` }}>${val}</li>`;
    });
  };

  return html`
    <div 
      class=${`analog-gauge ${className}`}
      style=${{
       '--analog-gauge-value': `${calculateDegree(value)}deg`
      }}
    >
      <div class="gauge"></div>
      <ul class="value-marks">
        ${generateValueMarks()}
      </ul>
      <div class="needle" style=${{ rotate: `${calculateDegree(value)}deg` }}></div>
      <div class="value">${value}${suffix}</div>
      <div class="label">${label}</div>
      <div class="label-min">${minLabel}</div>
      <div class="label-max">${maxLabel}</div>
    </div>
  `;
}