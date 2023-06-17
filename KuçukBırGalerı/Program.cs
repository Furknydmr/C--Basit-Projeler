using System;
using System.Collections.Generic;
using System.Linq;
using System.Management.Instrumentation;
using System.Text;
using System.Threading.Tasks;

namespace KuçukBırGalerı
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var araclar = new List<IArac>();
            // otomobil m otobus b
            //m|Marka|Model|Yas|1
            //b|Marka|Model|Yas|0
            // çıkış için ç
            Console.Clear();
            var idsayac = 1;
            while (true)
            {
                var girilenDeger = Console.ReadLine();
                if (girilenDeger=="ç")
                {
                    break;
                }
                var parcalar = girilenDeger.Split('|');
                var Marka = parcalar[1];
                var Model = parcalar[2];
                var yas = Convert.ToInt32(parcalar[3]);
                var ozeldeger = parcalar[4] == "1" ? true : false;
                if (parcalar[0]=="m")
                {
                    // otomobil
                    araclar.Add(new Otomobil
                    {
                        Id = idsayac,
                        Marka = Marka,
                        Model = Model,
                        Yas = yas,
                        UstuAcıkmı = ozeldeger
                    } );
                }
                else if (parcalar[0]=="b")
                    {
                    // otobüs
                    araclar.Add(new Otobus
                    {
                        Id = idsayac,
                        Marka = Marka,
                        Model = Model,
                        Yas = yas,
                        TakoMetreVarmı=ozeldeger
                    });
                }
                else
                {
                    Console.WriteLine("otomobil veya otobüs olduğunu belirtiniz!");
                }
                idsayac++;
            }
            foreach (var arac in araclar)
            {
                Console.WriteLine(arac.OzellıklerıGoster());
            }
            Console.ReadLine();
        }
        interface IArac
        {
            int Id { get; set; }
            string Marka { get; set; }
            string Model { get; set; }
            int Yas { get; set; }
            string OzellıklerıGoster();
        }
        class Otomobil : IArac
        {
            public int Id { get;set; }
            public string Marka { get; set; }
            public string Model { get; set; }
            public int Yas { get; set; }
            public bool UstuAcıkmı { get; set; }

            public string OzellıklerıGoster()
            {
                return $"Id: {Id}, Marka: {Marka}, Model:{Model}, Yas: {Yas}, Ustu Açık mı: {(UstuAcıkmı ? "evet" : "Hayır")}";
            }
        }
        class Otobus : IArac
        {
            public int Id { get; set; }
            public string Marka { get; set; }
            public string Model { get; set; }
            public int Yas { get; set; }
            public bool TakoMetreVarmı { get; set; }

            public string OzellıklerıGoster()
            {
                return $"Id: {Id}, Marka: {Marka}, Model:{Model}, Yas: {Yas}, Takometre Varmı: {(TakoMetreVarmı?"evet":"Hayır")}";
            }
        }
    }
}
