using System;
using System.Collections.Generic;
using System.Data.Entity.Core.Objects;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Web.Http;
using Trivio.Models;

namespace Trivio.Controllers
{
	[RoutePrefix("Trivia")]
	public class TriviaController : ApiController
	{

		//Dummy Trivia database-----------------------------------------------------------
		public static IEnumerable<Trivia> TriviaDb = new List<Trivia> {
			new Trivia { Id=1, Text="This was funny"},
			new Trivia { Id=2, Text="This was funnier"},
			new Trivia { Id=3, Text="This was funniest"}

		};

		//Dummy User Database
		public static IEnumerable<User> UserDb = new List<User> {
			new User {Id=1, FirstName="Neil", LastName ="Sanghrajka", Token=1, TriviaCount=0},
			new User {Id=2, FirstName="Disha", LastName ="Sanghrajka", Token=2, TriviaCount=0}
		};


		//------------Routes---------------------------------------------------------
		//GET Trivia/{UserId} 
		[HttpGet]
		[Route("{UserId:int}")] 
		public HttpResponseMessage GetTrivia(int UserId)
		{
			var User = FindUserById(UserId);
			
			//Check for User Id
			if (User != null)
			{
				//Find Trivia associated with UserId
				int NewTriviaId = User.GenerateNextTriviaId();
				Trivia NewTrivia = TriviaDb.SingleOrDefault(c => c.Id == NewTriviaId);
				
				//Check if end of all facts in username
				if (NewTrivia != null)
				{
					
					//Add that TriviaId to user history.
					User.TrivaHistory.Add(NewTriviaId);

					//Return new trivia for User
					return new HttpResponseMessage()
					{
						Content = new StringContent(NewTrivia.Text, Encoding.UTF8, "text/plain"),
						StatusCode = HttpStatusCode.OK
					};
				}
				
			}

			//If error
			return new HttpResponseMessage()
			{
				Content = new StringContent("Error", Encoding.UTF8, "text/plain"),
				StatusCode = HttpStatusCode.BadRequest
			};

		}



		//POST Trivia/{UserId}/{Count}
		// PUT Trivia/Vote


		//--------------------------Helper Functions----------------------------------------------
		//Check If User exists
		public static User FindUserById(int UserId)
		{
			return UserDb.SingleOrDefault(c => c.Id == UserId);
		}

    }



}
