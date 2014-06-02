//
// GenreMap.cs
//
// This file is part of PeerProject (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)

#region Using directives
using System;
using System.Collections;
using System.Text;

#endregion

namespace Schemas
{
	internal static class GenreMap
	{
		static Hashtable _map = new Hashtable();
		static GenreMap()
		{
			_map.Add(genreType.adv_animal, PeerProjectBook.PeerGenreType.OutdoorsNature);
			_map.Add(genreType.adv_geo, PeerProjectBook.PeerGenreType.Travel);
			_map.Add(genreType.adv_history, PeerProjectBook.PeerGenreType.History);
			_map.Add(genreType.adv_indian, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.adv_maritime, PeerProjectBook.PeerGenreType.Travel);
			_map.Add(genreType.adv_western, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.adventure, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.antique, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.antique_ant, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.antique_east, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.antique_european, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.antique_myths, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.antique_russian, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.child_adv, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.child_det, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.child_education, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.child_prose, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.child_sf, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.child_tale, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.child_verse, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.children, PeerProjectBook.PeerGenreType.ChildrensBooks);
			_map.Add(genreType.comp_db, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.comp_hard, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.comp_osnet, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.comp_programming, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.comp_soft, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.comp_www, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.computers, PeerProjectBook.PeerGenreType.ComputersInternet);
			_map.Add(genreType.design, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.det_action, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.det_classic, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.det_crime, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.det_espionage, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.det_hard, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.det_history, PeerProjectBook.PeerGenreType.History);
			_map.Add(genreType.det_irony, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.det_maniac, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.det_police, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.det_political, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.detective, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.dramaturgy, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.home, PeerProjectBook.PeerGenreType.HomeGarden);
			_map.Add(genreType.home_cooking, PeerProjectBook.PeerGenreType.CookingFoodWine);
			_map.Add(genreType.home_crafts, PeerProjectBook.PeerGenreType.HomeGarden);
			_map.Add(genreType.home_diy, PeerProjectBook.PeerGenreType.HomeGarden);
			_map.Add(genreType.home_entertain, PeerProjectBook.PeerGenreType.Entertainment);
			_map.Add(genreType.home_garden, PeerProjectBook.PeerGenreType.HomeGarden);
			_map.Add(genreType.home_health, PeerProjectBook.PeerGenreType.HealthFitness);
			_map.Add(genreType.home_pets, PeerProjectBook.PeerGenreType.HomeGarden);
			_map.Add(genreType.home_sex, PeerProjectBook.PeerGenreType.HealthFitness);
			_map.Add(genreType.home_sport, PeerProjectBook.PeerGenreType.Sports);
			_map.Add(genreType.humor, PeerProjectBook.PeerGenreType.Comics);
			_map.Add(genreType.humor_anecdote, PeerProjectBook.PeerGenreType.Comics);
			_map.Add(genreType.humor_prose, PeerProjectBook.PeerGenreType.Comics);
			_map.Add(genreType.humor_verse, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.love_contemporary, PeerProjectBook.PeerGenreType.Romance);
			_map.Add(genreType.love_detective, PeerProjectBook.PeerGenreType.MysteryThrillers);
			_map.Add(genreType.love_erotica, PeerProjectBook.PeerGenreType.Romance);
			_map.Add(genreType.love_history, PeerProjectBook.PeerGenreType.History);
			_map.Add(genreType.love_short, PeerProjectBook.PeerGenreType.Romance);
			_map.Add(genreType.nonf_biography, PeerProjectBook.PeerGenreType.BiographiesMemoirs);
			_map.Add(genreType.nonf_criticism, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.nonf_publicism, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.nonfiction, PeerProjectBook.PeerGenreType.Nonfiction);
			_map.Add(genreType.poetry, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.prose_classic, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.prose_contemporary, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.prose_counter, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.prose_history, PeerProjectBook.PeerGenreType.History);
			_map.Add(genreType.prose_rus_classic, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.prose_su_classics, PeerProjectBook.PeerGenreType.LiteratureFiction);
			_map.Add(genreType.ref_dict, PeerProjectBook.PeerGenreType.Reference);
			_map.Add(genreType.ref_encyc, PeerProjectBook.PeerGenreType.Reference);
			_map.Add(genreType.ref_guide, PeerProjectBook.PeerGenreType.Reference);
			_map.Add(genreType.ref_ref, PeerProjectBook.PeerGenreType.Reference);
			_map.Add(genreType.reference, PeerProjectBook.PeerGenreType.Reference);
			_map.Add(genreType.religion, PeerProjectBook.PeerGenreType.ReligionSpirituality);
			_map.Add(genreType.religion_esoterics, PeerProjectBook.PeerGenreType.ReligionSpirituality);
			_map.Add(genreType.religion_rel, PeerProjectBook.PeerGenreType.ReligionSpirituality);
			_map.Add(genreType.religion_self, PeerProjectBook.PeerGenreType.MindBody);			
			_map.Add(genreType.sci_biology, PeerProjectBook.PeerGenreType.Science);
			_map.Add(genreType.sci_business, PeerProjectBook.PeerGenreType.BusinessInvesting);
			_map.Add(genreType.sci_chem, PeerProjectBook.PeerGenreType.Science);
			_map.Add(genreType.sci_culture, PeerProjectBook.PeerGenreType.Science);
			_map.Add(genreType.sci_history, PeerProjectBook.PeerGenreType.History);
			_map.Add(genreType.sci_juris, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_linguistic, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_math, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_medicine, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_philosophy, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_phys, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_politics, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_psychology, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.sci_religion, PeerProjectBook.PeerGenreType.ReligionSpirituality);
			_map.Add(genreType.sci_tech, PeerProjectBook.PeerGenreType.ProfessionalTechnical);
			_map.Add(genreType.science, PeerProjectBook.PeerGenreType.Science);
			_map.Add(genreType.sf, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_action, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_cyberpunk, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_detective, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_epic, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_fantasy, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_heroic, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_history, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_horror, PeerProjectBook.PeerGenreType.Horror);
			_map.Add(genreType.sf_humor, PeerProjectBook.PeerGenreType.Comics);
			_map.Add(genreType.sf_social, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.sf_space, PeerProjectBook.PeerGenreType.ScienceFictionFantasy);
			_map.Add(genreType.thriller, PeerProjectBook.PeerGenreType.MysteryThrillers);
	}
		
		public static PeerProjectBook.PeerGenreType GetPeerGenre(genreType fbGenre) {
			return (PeerProjectBook.PeerGenreType)_map[fbGenre];
		}
	}
}
