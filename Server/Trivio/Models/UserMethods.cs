using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace Trivio.Models
{
	public partial class User
	{
		public int GenerateNextTriviaId()
		{
			//Update trivia count in database
			++TriviaCount;

			//Return Id of next trivia

			return TriviaCount;
		}
	}
}