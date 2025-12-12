import React, { useEffect, useState } from "react";
import { Button } from "./components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "./components/ui/card";
import { Input } from "./components/ui/input";
import { Label } from "./components/ui/label";
import Logo from "./assets/logo.png";

type Candidate = { id: string; name: string; img: string };

const presidentCandidates: Candidate[] = [
  { id: "4CB22CB001", name: "Alice Johnson", img: "https://randomuser.me/api/portraits/women/1.jpg" },
  { id: "4CB22CB002", name: "Bob Smith", img: "https://randomuser.me/api/portraits/men/2.jpg" },
  { id: "4CB22CB003", name: "Carol White", img: "https://randomuser.me/api/portraits/women/3.jpg" },
  { id: "4CB22CB004", name: "David Brown", img: "https://randomuser.me/api/portraits/men/4.jpg" },
];

const vicePresidentCandidates: Candidate[] = [
  { id: "4CB22CB005", name: "Eve Green", img: "https://randomuser.me/api/portraits/women/5.jpg" },
  { id: "4CB22CB006", name: "Frank Black", img: "https://randomuser.me/api/portraits/men/6.jpg" },
  { id: "4CB22CB007", name: "Grace Lee", img: "https://randomuser.me/api/portraits/women/7.jpg" },
  { id: "4CB22CB008", name: "Henry King", img: "https://randomuser.me/api/portraits/men/8.jpg" },
];

const generalSecretaryCandidates: Candidate[] = [
  { id: "4CB22CB009", name: "Ivy Adams", img: "https://randomuser.me/api/portraits/women/9.jpg" },
  { id: "4CB22CB010", name: "Jack White", img: "https://randomuser.me/api/portraits/men/10.jpg" },
  { id: "4CB22CB011", name: "Karen Young", img: "https://randomuser.me/api/portraits/women/11.jpg" },
  { id: "4CB22CB012", name: "Leo Gray", img: "https://randomuser.me/api/portraits/men/12.jpg" },
];

const jointSecretaryCandidates: Candidate[] = [
  { id: "4CB22CB013", name: "Mia Scott", img: "https://randomuser.me/api/portraits/women/13.jpg" },
  { id: "4CB22CB014", name: "Noah Hill", img: "https://randomuser.me/api/portraits/men/14.jpg" },
  { id: "4CB22CB015", name: "Olivia Clark", img: "https://randomuser.me/api/portraits/women/15.jpg" },
  { id: "4CB22CB016", name: "Peter Lewis", img: "https://randomuser.me/api/portraits/men/16.jpg" },
];

const culturalSecretaryCandidates: Candidate[] = [
  { id: "4CB22CB017", name: "Quinn Wright", img: "https://randomuser.me/api/portraits/women/17.jpg" },
  { id: "4CB22CB018", name: "Rachel Adams", img: "https://randomuser.me/api/portraits/women/18.jpg" },
  { id: "4CB22CB019", name: "Sam Baker", img: "https://randomuser.me/api/portraits/men/19.jpg" },
  { id: "4CB22CB020", name: "Tina Clark", img: "https://randomuser.me/api/portraits/women/20.jpg" },
];

const sportsSecretaryCandidates: Candidate[] = [
  { id: "4CB22CB021", name: "Uma Patel", img: "https://randomuser.me/api/portraits/women/21.jpg" },
  { id: "4CB22CB022", name: "Victor Singh", img: "https://randomuser.me/api/portraits/men/22.jpg" },
  { id: "4CB22CB023", name: "Will Johnson", img: "https://randomuser.me/api/portraits/men/23.jpg" },
  { id: "4CB22CB024", name: "Xavier Thomas", img: "https://randomuser.me/api/portraits/men/24.jpg" },
];

/* -------------------------
   Strongly typed position names
   ------------------------- */
type PositionTitle =
  | "President"
  | "Vice President"
  | "General Secretary"
  | "Joint Secretary"
  | "Cultural Secretary"
  | "Sports Secretary";

type Position = { title: PositionTitle; candidates: Candidate[] };

const positions: Position[] = [
  { title: "President", candidates: presidentCandidates },
  { title: "Vice President", candidates: vicePresidentCandidates },
  { title: "General Secretary", candidates: generalSecretaryCandidates },
  { title: "Joint Secretary", candidates: jointSecretaryCandidates },
  { title: "Cultural Secretary", candidates: culturalSecretaryCandidates },
  { title: "Sports Secretary", candidates: sportsSecretaryCandidates },
];

/* -------------------------
   Selections typing
   ------------------------- */
type Selections = Record<PositionTitle, string | null>;

/* -------------------------
   Component
   ------------------------- */
const ADMIN_PASSWORD = "admin@123";

const VotingSystem: React.FC = () => {
  // initialize selections with known position keys (so TS knows keys)
  const initialSelections = positions.reduce<Selections>((acc, pos) => {
    acc[pos.title] = null;
    return acc;
  }, {
    "President": null,
    "Vice President": null,
    "General Secretary": null,
    "Joint Secretary": null,
    "Cultural Secretary": null,
    "Sports Secretary": null,
  });

  const [isVotingActive, setIsVotingActive] = useState<boolean>(false);
  const [timer, setTimer] = useState<number>(0);
  const [selectedCandidates, setSelectedCandidates] = useState<Selections>(initialSelections);
  const [password, setPassword] = useState<string>("");
  const [hasVoted, setHasVoted] = useState<boolean>(false);
  const [isVotingEnded, setIsVotingEnded] = useState<boolean>(false);

  useEffect(() => {
    let interval: number | undefined;
    if (isVotingActive && !isVotingEnded) {
      interval = window.setInterval(() => setTimer((t) => t + 1), 1000);
    }
    return () => {
      if (interval) window.clearInterval(interval);
    };
  }, [isVotingActive, isVotingEnded]);

  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins.toString().padStart(2, "0")}:${secs.toString().padStart(2, "0")}`;
  };

  const handleSelectCandidate = (position: PositionTitle, candidateId: string) => {
    if (!isVotingActive || hasVoted || isVotingEnded) return; // guard
    setSelectedCandidates((prev) => ({ ...prev, [position]: candidateId }));
  };

  const handleSubmitVote = async () => {
    if (isVotingEnded) {
      alert("❌ Voting has ended. No more votes accepted.");
      return;
    }
    if (!isVotingActive) {
      alert("❌ Voting is not active.");
      return;
    }

    // validate selections
    for (const pos of positions) {
      if (!selectedCandidates[pos.title]) {
        alert(`❌ Please select a candidate for ${pos.title}`);
        return;
      }
    }

    if (!password) {
      alert("❌ Please enter password!");
      return;
    }

    try {
      const response = await fetch("http://127.0.0.1:5000/submit_vote", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ votes: selectedCandidates, password }),
      });

      const data = await response.json();

      if (!response.ok) {
        alert(`❌ ${data?.error || "Vote submission failed"}`);
        return;
      }

      alert(`✅ ${data?.message || "Vote submitted successfully"}`);
      setHasVoted(true);
      setPassword("");
    } catch (err) {
      console.error(err);
      alert("❌ Could not connect to backend");
    }
  };

  const handleStopVoting = () => {
    setIsVotingActive(false);
    setSelectedCandidates(initialSelections);
    setHasVoted(false);
    setPassword("");
    setTimer(0);
  };

  const handleEndVoting = () => {
    // End Voting should be available anytime (as requested).
    const pass = prompt("Enter admin password to end voting:");
    if (pass === ADMIN_PASSWORD) {
      setIsVotingEnded(true);
      setIsVotingActive(false);
      alert("✅ Voting has been ended by admin.");
    } else {
      alert("❌ Incorrect admin password. Voting not ended.");
    }
  };

  // Voting ended view
  if (isVotingEnded) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-gray-100 p-6">
        <div className="bg-white p-10 rounded-xl shadow-xl text-center max-w-lg w-full">
          <img src={Logo} alt="Logo" className="mx-auto mb-4 w-20 h-20 object-contain rounded-full" />
          <h1 className="text-4xl font-bold text-red-600 mb-4">Voting Ended</h1>
          <p className="text-lg text-gray-700 mb-6">No more votes can be submitted.</p>
          <p className="text-sm text-gray-500">If you need to re-open voting, use the admin panel (not implemented here).</p>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-50 py-8 px-4">
      <div className="max-w-6xl mx-auto bg-white rounded-xl shadow-lg overflow-hidden">

        <header className="bg-blue-800 text-white p-6 text-center">
          <div className="flex items-center justify-center mb-4">
            <img src={Logo} alt="Logo" className="mr-4 rounded-full border-4 border-white w-24 h-24 object-contain" />
            <h1 className="text-3xl font-bold">CANARA ENGINEERING COLLEGE</h1>
          </div>
          <h2 className="text-2xl font-semibold mt-2">Student Association Election</h2>
          <p className="text-lg mt-1">College-Wide Voting Portal</p>
        </header>

        <section className="p-6 bg-gray-100 border-b flex flex-col md:flex-row items-center justify-between gap-4">
          <div className="flex gap-4">
            <Button onClick={() => setIsVotingActive(true)} disabled={isVotingActive}>
              Start Voting
            </Button>
            <Button onClick={handleStopVoting} disabled={!isVotingActive}>
              Stop Voting
            </Button>
          </div>

          <div className="bg-white p-4 rounded-lg shadow-md text-center">
            <p className="text-lg font-semibold">Voting Timer: {formatTime(timer)}</p>
            <p className={`text-sm ${isVotingActive ? "text-green-600" : "text-red-600"}`}>
              Status: {isVotingActive ? "Voting Active" : "Voting Inactive"}
            </p>
          </div>
        </section>

        <section className="p-6">
          {positions.map((pos) => (
            <div key={pos.title} className="mb-12">
              <h3 className="text-2xl font-bold mb-6 text-center border-b-2 pb-2">{pos.title}</h3>

              <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
                {pos.candidates.map((c) => (
                  <Card
                    key={c.id}
                    className={`text-center transition-all ${selectedCandidates[pos.title] === c.id ? "ring-2 ring-black" : ""}`}
                  >
                    <CardHeader>
                      <img src={c.img} alt={c.name} className="w-32 h-32 rounded-full mx-auto object-cover" />
                    </CardHeader>
                    <CardContent>
                      <CardTitle className="text-lg">{c.name}</CardTitle>

                      <Button
                        onClick={() => handleSelectCandidate(pos.title, c.id)}
                        disabled={!isVotingActive || hasVoted || isVotingEnded}
                        className="mt-4 bg-blue-600 hover:bg-blue-700"
                      >
                        {selectedCandidates[pos.title] === c.id ? "Selected" : "Vote"}
                      </Button>
                    </CardContent>
                  </Card>
                ))}
              </div>
            </div>
          ))}
        </section>

        <section className="p-6 flex flex-col md:flex-row items-center justify-center gap-4 bg-gray-100 border-t">
          <Label htmlFor="password">Enter Password:</Label>

          <Input
            type="password"
            id="password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            disabled={!isVotingActive || hasVoted || isVotingEnded}
            className="w-64"
          />

          <Button
            onClick={handleSubmitVote}
            disabled={!isVotingActive || hasVoted || isVotingEnded}
            className="px-6 py-3 bg-green-600 hover:bg-green-700"
          >
            Submit Vote
          </Button>
        </section>

        {/* End Voting button (placed at bottom per option B) */}
        <section className="p-6 bg-red-50 text-center border-t">
          {/* Button is always clickable until voting actually ends */}
          <button
            onClick={handleEndVoting}
            className="bg-red-600 hover:bg-red-700 text-white px-6 py-3 rounded-lg font-bold shadow-md"
          >
            End Voting
          </button>
        </section>

      </div>
    </div>
  );
};

export default VotingSystem;
