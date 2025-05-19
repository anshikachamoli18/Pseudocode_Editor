import '../styles/global.css';

export const metadata = {
  title: 'Pseudocode IDE',
  description: 'Write and run your pseudocode in real time.',
};

export default function RootLayout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="en">
      <body>{children}</body>
    </html>
  );
}
