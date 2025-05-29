import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  output: "export", // Required for static export
  distDir: "../out", // Explicitly set output directory

  webpack(config) {
    config.module.rules.push({
      test: /\.svg$/,
      use: ["@svgr/webpack"],
    });
    return config;
  },
};

export default nextConfig;

