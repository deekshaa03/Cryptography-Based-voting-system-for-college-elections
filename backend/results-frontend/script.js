// const API_URL = "/results"; // Flask endpoint
const API_URL = "http://127.0.0.1:5000/results";
const resultsDiv = document.getElementById("results");

fetch(API_URL)
  .then(response => {
    if (!response.ok) throw new Error("Network response was not ok");
    return response.json();
  })
  .then(data => displayResults(data))  // your backend already returns JSON array
  .catch(error => {
    resultsDiv.innerHTML = `<p style="color:red; text-align:center;">Failed to load results: ${error}</p>`;
  });

function displayResults(data) {
  resultsDiv.innerHTML = ""; // Clear any previous content

  data.forEach(positionObj => {
    const div = document.createElement("div");
    div.className = "position";

    const h2 = document.createElement("h2");
    h2.textContent = positionObj.position;
    div.appendChild(h2);

    const ul = document.createElement("ul");

    const votes = positionObj.votes;
    const maxVotes = Math.max(...Object.values(votes));

    for (const [candidate, voteCount] of Object.entries(votes)) {
      const li = document.createElement("li");
      li.textContent = `${candidate}: ${voteCount} votes`;

      // highlight winners
      if (voteCount === maxVotes) li.classList.add("winner");

      ul.appendChild(li);
    }

    div.appendChild(ul);
    resultsDiv.appendChild(div);
  });
}
