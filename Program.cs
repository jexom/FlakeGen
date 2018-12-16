using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace FlakeGen
{
    //14x12
    class Program
    {
        private static void drawTextProgressBar(int progress, int total)
        {
            //draw empty progress bar
            Console.CursorLeft = 0;
            Console.Write("["); //start
            Console.CursorLeft = 32;
            Console.Write("]"); //end
            Console.CursorLeft = 1;
            float onechunk = 30.0f / total;

            //draw filled part
            int position = 1;
            for (int i = 0; i < onechunk * progress; i++)
            {
                Console.CursorLeft = position++;
                Console.Write("=");
            }

            Console.CursorLeft = position++;
            Console.Write(">");

            //draw unfilled part
            for (int i = position; i <= 31; i++)
            {
                Console.CursorLeft = position++;
                Console.Write(" ");
            }

            //draw totals
            Console.CursorLeft = 35;

            Console.Write("Processing line " + progress.ToString() + " of " + total.ToString()); //blanks at the end remove any excess
        }

        static int[,] d = {
                {0,0,0,1,1,1,1,1,1,1,1,0,0,0 },
                {0,0,1,1,1,1,1,1,1,1,1,1,0,0 },
                {0,0,1,1,1,1,1,1,1,1,1,1,0,0 },
                {0,1,1,1,1,1,1,1,1,1,1,1,1,0 },
                {0,1,1,1,1,1,1,1,1,1,1,1,1,0 },
                {1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
                {1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
                {0,1,1,1,1,1,1,1,1,1,1,1,1,0 },
                {0,1,1,1,1,1,1,1,1,1,1,1,1,0 },
                {0,0,1,1,1,1,1,1,1,1,1,1,0,0 },
                {0,0,1,1,1,1,1,1,1,1,1,1,0,0 },
                {0,0,0,1,1,1,1,1,1,1,1,0,0,0 }
            };

        static void Main(string[] args)
        {
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(OnProcessExit);

            Console.CursorVisible = false;
            
            string[] lines = File.ReadAllLines("example.txt");

            //string[] lines = new string[4];

            //lines[0] = "3";
            //lines[1] = "0\t255\t255";
            //lines[2] = "255\t255\t255";
            //lines[3] = "0\t255\t255";

            string ws = lines[0];
            int w;
            int.TryParse(ws, out w);

            Bitmap bm = new Bitmap((int)((w + 0.5) * 12), w * 11 + 3);
            Graphics gp = Graphics.FromImage(bm);
            gp.Clear(Color.Black);

            for (int j = 0; j < w; j++)
            {
                drawTextProgressBar(j, w);
                string[] row = lines[j + 1].Split('\t');
                for (int i = 0; i < w; i++)
                {
                    int g;
                    int.TryParse(row[i], out g);
                    if (g > 255)
                        g = 255;
                    Color col = Color.FromArgb(g, g, g);

                    int x = i * 12 + (6 * (j % 2));
                    int y = j * 11;

                    for (int pj = 0; pj < 14; pj++)
                    {
                        for (int pi = 0; pi < 12; pi++)
                        {
                            if (d[pi, pj] == 1)
                                bm.SetPixel(x + pi, y + pj, col);
                        }
                    }
                }
            }

            string dt = DateTime.Now.ToString();
            dt = dt.Replace(".","");
            dt = dt.Replace(":", "");
            dt = dt.Replace(" ", "-");

            bm.Save("snowflake-"+ dt +".png");
        }

        static void OnProcessExit(object sender, EventArgs e)
        {
            Console.CursorVisible = true;
        }
    }
}
