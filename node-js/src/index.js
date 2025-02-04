#!/usr/bin/env node

const { Console } = require("console");
const { Transform } = require("stream");

function table(input) {
  // @see https://stackoverflow.com/a/67859384
  const ts = new Transform({
    transform(chunk, enc, cb) {
      cb(null, chunk);
    },
  });
  const logger = new Console({ stdout: ts });
  logger.table(input);
  const table = (ts.read() || "").toString();
  let result = "";
  for (let row of table.split(/[\r\n]+/)) {
    let r = row.replace(/[^┬]*┬/, "┌");
    r = r.replace(/^├─*┼/, "├");
    r = r.replace(/│[^│]*/, "");
    r = r.replace(/^└─*┴/, "└");
    r = r.replace(/'/g, " ");
    result += `${r}\n`;
  }
  console.log(result);
}

async function getMeals() {
  const d = new Date();
  const month = `${d.getMonth() + 1}`.padStart(2, "0");

  const url = `https://mensa.aaronschlitt.de/meals/Griebnitzsee?date=${d.getFullYear()}-${month}-${d.getDate()}&lang=de`;
  const res = await fetch(url);
  const data = await res.json();
  const meals = data.map((d) => new Meal(d));
  meals.sort((a, b) => parseInt(b.price) - parseInt(a.price));
  return meals;
}

class Meal {
  constructor(data) {
    this.data = data;
  }

  get shouldPrint() {
    return !this.data.isEveningMeal && this.data.name !== "Preise pro 100 g";
  }

  get labels() {
    return this.data.features.map((e) => e.abbreviation).join(" ");
  }

  get price() {
    return this.data.studentPrice.toFixed(2);
  }

  get name() {
    return `${this.data.name} (${this.labels})`;
  }
}

async function main() {
  const meals = await getMeals();
  const all = process.argv.includes("all");
  const asTable = !process.argv.includes("notable");
  const toPrint = meals.filter(
    (m) => m.shouldPrint || (all && !m.data.isEveningMeal)
  );
  if (asTable) table(toPrint.map((e) => ({ Gericht: e.name, Preis: e.price })));
  else meals.forEach((e) => console.log(`(${e.price}) ${e.name}`));
}

(async () => await main())();
