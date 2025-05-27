'use client';
import React from 'react';

export default function Navbar() {
  return (
    <>
      <style>
        {`
          .navbar {
            padding: 1rem 1.5rem;
            border-bottom: 1px solid #4b5563; /* gray-800 */
            background: linear-gradient(to right, #4c1d95, #4338ca); /* purple-900 to indigo-900 */
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); /* shadow-md */
            color: white;
          }

          .navbar h1 {
            font-size: 1.875rem; /* text-3xl */
            font-weight: bold;
            display: flex;
            align-items: center;
            gap: 0.75rem;
            margin: 0;
            font-family:'Times New Roman', Times, serif; 
          }

        `}
      </style>

      <header className="navbar">
        <h1>
          🚀 PseudoX: Pseudocode Execution Platform
        </h1>
      </header>
    </>
  );
}
